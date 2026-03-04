modded class PluginLifespan
{
	protected int GetTerjePersistentBeardMaxLevel(PlayerBase player)
	{
		if (player == null) return 0;
		if (player.GetTerjeProfile() == null) return 0;

		int selectedMax = TerjeMathHelper.ClampInt(player.GetTerjeProfile().GetPersistentBeardLevelSelected(), 0, 3);
		return selectedMax;
	}

	void SetPersistentBeardLevel(PlayerBase player, int level)
	{
		if (player == null) return;

		array<ref LifespanLevel> levels = m_LifespanLevels.Get(player.GetPlayerClass());
		if (levels == null || levels.Count() == 0) return;

		int maxBySettings = GetTerjePersistentBeardMaxLevel(player);
		int maxByModel = levels.Count() - 1;
		int maxLevel = maxBySettings;
		if (maxLevel > maxByModel) maxLevel = maxByModel;

		int selectedLevel = TerjeMathHelper.ClampInt(level, 0, maxLevel);
		LifespanLevel visualLevel = null;
		for (int i = 0; i < levels.Count(); i++)
		{
			LifespanLevel candidateLevel = levels.Get(i);
			if (candidateLevel && candidateLevel.GetLevel() == selectedLevel)
			{
				visualLevel = candidateLevel;
				break;
			}
		}

		if (visualLevel == null && selectedLevel >= 0 && selectedLevel < levels.Count()) visualLevel = levels.Get(selectedLevel);
		if (visualLevel == null) return;

		SetPlayerLifespanLevel(player, visualLevel);
		m_PlayerCurrentLevel.Set(player, visualLevel);
	}

	override void UpdateLifespan(PlayerBase player, bool force_update = false)
	{
		super.UpdateLifespan(player, force_update);
		if (!GetGame() || !GetGame().IsDedicatedServer()) return;
		if (!GetTerjeSettingBool(TerjeSettingsCollection.STARTSCREEN_PBEARD_ENABLED)) return;
		if (player == null || player.GetTerjeProfile() == null) return;

		TerjePlayerProfile profile = player.GetTerjeProfile();
		bool allowShave = GetTerjeSettingBool(TerjeSettingsCollection.STARTSCREEN_PBEARD_ALLOWSHAVE);

		int maxLevel = GetTerjePersistentBeardMaxLevel(player);
		int currentLevel = TerjeMathHelper.ClampInt(player.GetLifeSpanState(), 0, maxLevel);
		if (player.GetLifeSpanState() > maxLevel)
		{
			SetPersistentBeardLevel(player, maxLevel);
			currentLevel = maxLevel;
		}

		if (allowShave)
		{
			return;
		}

		int lockedLevel = TerjeMathHelper.ClampInt(profile.GetPersistentBeardLevelSelected(), 0, maxLevel);
		if (currentLevel != lockedLevel) SetPersistentBeardLevel(player, lockedLevel);
	}
}
