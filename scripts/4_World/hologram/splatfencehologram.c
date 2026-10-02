/*
	modded Hologram — SPLATFence placement range/height loosening
	------------------------------------------------------------
	Lets you stand in front of an existing Indestructible Wall, look up at its
	top, and place another kit up there. Every override below is gated on
	SPLATFence_IsKit() (the item in hands is a SPLATFenceKitDouble) and calls
	super for everything else, so no other deployable in the game is affected.

	Hologram is a single global class - PlayerBase does `new Hologram(this, ...,
	item)` with no per-item hook - so `modded class` + a type gate is the only
	way to do this. m_Parent is the kit in hands; m_Projection is the ghost
	(SPLATFenceKitDoublePlacing).

	Three vanilla limits are in the way (all in scripts/4_world/classes/hologram.c,
	build 124708):

	1. Hologram.IsClippingRoof() clause b1: "projection Y > camera Y" -> can't
	   place anything above eye level. Client-only (skipped on MP servers), but
	   the deploy action's ActionCondition gates on the client hologram's
	   IsColliding(), so it blocks you from ever starting the placement.

	2. Hologram.HeightPlacementCheck(): |playerY - projectionY| > 1.5 (hard-coded
	   DEFAULT_MAX_PLACEMENT_HEIGHT_DIFF). Runs on BOTH client and server.

	3. Hologram.GetProjectionEntityPosition(): the ghost is clamped to
	   projectionRadius .. min(projectionRadius*2, 6) metres from the player; at
	   the far edge SetHologramPosition() reports "floating", which
	   EvaluateCollision() treats as colliding. For the fence's placing model
	   that upper bound works out to only ~2-3 m - the "nose to the wall".

	NOT touched (leave for testing / server config if they get in the way):
	  - IsBaseViable()/IsBaseStatic()/IsBaseFlat() - the 4-corner "is there solid
	    flat static ground under this" check. Client-only on MP servers. If
	    stacking on the wall top trips it, add an override here too.
*/

// How far (m) the fence ghost may sit from the player. Vanilla caps most things
// at LARGE_PROJECTION_DISTANCE_LIMIT (6) and the fence's own projectionRadius*2
// makes it far shorter (~2-3 m). Tune this for reach.
const float SPLATFENCE_MAX_PROJECTION_DISTANCE = 3.5;

modded class Hologram
{
	protected bool SPLATFence_IsKit()
	{
		return m_Parent && m_Parent.IsKindOf("SPLATFenceKitDouble");
	}

	// --- limit 2: the +/-1.5 m player-to-ghost height cap
	override bool HeightPlacementCheck()
	{
		if (SPLATFence_IsKit())
			return true;

		return super.HeightPlacementCheck();
	}

	// --- limit 1: "can't place above eye level"
	// Faithful copy of vanilla Hologram.IsClippingRoof() MINUS the b1 clause
	// (projection above camera). The real roof check (b2) is kept, so you still
	// can't place a kit inside a building ceiling.
	override bool IsClippingRoof()
	{
		if (!SPLATFence_IsKit())
			return super.IsClippingRoof();

		if (CfgGameplayHandler.GetDisableIsClippingRoofCheck())
			return false;

		if (g_Game.IsServer() && g_Game.IsMultiplayer())
			return false;

		if (m_Projection && m_Projection.DoPlacingHeightCheck())
			return MiscGameplayFunctions.IsUnderRoofEx(m_Projection, GameConstants.ROOF_CHECK_RAYCAST_DIST, ObjIntersectFire);

		return false;
	}

	// --- limit 3: projection distance clamp, + hide the ghost when aiming at open air
	// Faithful copy of vanilla Hologram.GetProjectionEntityPosition() with two
	// changes for the fence kit:
	//   * maxProjectionDistance = SPLATFENCE_MAX_PROJECTION_DISTANCE (flat), instead
	//     of min(projectionRadius*2, 6). min is left as vanilla so the ghost still
	//     can't clip into you.
	//   * if the placement raycast hits nothing at all (aiming at sky / above a
	//     wall into empty air), return "0 0 0" so the projection sits at map origin
	//     - not visible, and IsCollidingZeroPos() blocks placement. This is the
	//     same trick vanilla's HideWhenClose() uses for looking straight up.
	// Keep this in sync if BI changes the original in a future build.
	override protected vector GetProjectionEntityPosition(PlayerBase player)
	{
		if (!SPLATFence_IsKit())
			return super.GetProjectionEntityPosition(player);

		float minProjectionDistance;
		float maxProjectionDistance;
		m_ContactDir = vector.Zero;
		float projectionRadius = GetProjectionRadius();
		float cameraToPlayerDistance = vector.Distance(g_Game.GetCurrentCameraPosition(), player.GetPosition());

		// min is left as vanilla (so the ghost still can't clip into you); only
		// the max reach is widened, to SPLATFENCE_MAX_PROJECTION_DISTANCE flat.
		if (projectionRadius < SMALL_PROJECTION_RADIUS)
			minProjectionDistance = SMALL_PROJECTION_RADIUS;
		else
			minProjectionDistance = projectionRadius;

		maxProjectionDistance = SPLATFENCE_MAX_PROJECTION_DISTANCE;

		vector from = g_Game.GetCurrentCameraPosition();
		vector to = from + (g_Game.GetCurrentCameraDirection() * (maxProjectionDistance + cameraToPlayerDistance));
		vector contactPosition;
		set<Object> hitObjects = new set<Object>();

		// RaycastRV returns true if it hit ANYTHING (terrain OR an object); hitObjects
		// only ever contains scripted objects, never terrain - so the bool is the
		// "did we hit a surface at all" signal, not hitObjects.Count().
		bool rayHit = DayZPhysics.RaycastRV(from, to, contactPosition, m_ContactDir, m_ContactComponent, hitObjects, player, m_Projection, false, false, ObjIntersectFire);

		bool contactHitProcessed = false;
		//! will not push hologram up when there is direct hit of an item
		if (!CfgGameplayHandler.GetDisableIsCollidingBBoxCheck())
		{
			if (hitObjects.Count() > 0)
			{
				if (hitObjects[0].IsInherited(Watchtower))
				{
					contactHitProcessed = true;
					contactPosition = CorrectForWatchtower(m_ContactComponent, contactPosition, player, hitObjects[0]);
				}

				if (!contactHitProcessed && hitObjects[0].IsInherited(InventoryItem))
					contactPosition = hitObjects[0].GetPosition();
			}
		}

		static const float raycastOriginOffsetOnFail = 0.25;
		static const float minDistFromStart = 0.01;
		// Camera isn't correctly positioned in some cases, leading to raycasts hitting the object directly behind the camera
		if ((hitObjects.Count() > 0) && (vector.DistanceSq(from, contactPosition) < minDistFromStart))
		{
			from = contactPosition + g_Game.GetCurrentCameraDirection() * raycastOriginOffsetOnFail;
			rayHit = DayZPhysics.RaycastRV(from, to, contactPosition, m_ContactDir, m_ContactComponent, hitObjects, player, m_Projection, false, false, ObjIntersectFire);
		}

		// Aiming at nothing at all - no terrain, no object (sky, or over a wall into
		// empty air): park the projection at map origin so it isn't rendered, and
		// let IsCollidingZeroPos() block placement. Bare ground still counts as a
		// hit (rayHit == true even though hitObjects is empty for terrain).
		if (!rayHit)
		{
			SetIsFloating(false);
			m_FromAdjusted = from;
			return vector.Zero;
		}

		bool isFloating = SetHologramPosition(player.GetPosition(), minProjectionDistance, maxProjectionDistance, contactPosition);
		SetIsFloating(isFloating);

		m_FromAdjusted = from;

		return contactPosition;
	}
}
