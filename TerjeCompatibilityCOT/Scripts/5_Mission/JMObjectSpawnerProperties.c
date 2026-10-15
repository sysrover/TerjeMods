modded class JMObjectSpawnerProperties
{
	protected override void CollectLiquids( string classname, ItemBase item, out array<string> labels )
	{
		super.CollectLiquids(classname, item, labels);

		if (!item || !labels || !m_LiquidValues || classname == "" || item.IsBloodContainer())
			return;

		TerjeCustomLiquids uam_Liquids = TerjeCustomLiquids.GetInstance();
		if (!uam_Liquids || !uam_Liquids.m_LiquidInfosByType)
			return;

		int uam_DefaultLiquid = item.GetTerjeCustomLiquidTypeInit();
		foreach (int uam_LiquidType, LiquidInfo uam_LiquidInfo : uam_Liquids.m_LiquidInfosByType)
		{
			if (!uam_LiquidInfo)
				continue;

			string uam_DisplayName = Widget.TranslateString(uam_LiquidInfo.m_LiquidDisplayName);
			if (uam_LiquidType == uam_DefaultLiquid)
			{
				if (labels.Count() > 0)
					labels[0] = DefaultLabel(uam_DisplayName);
				continue;
			}

			m_LiquidValues.Insert(-uam_LiquidType);
			labels.Insert(uam_DisplayName);
		}
	}
}
