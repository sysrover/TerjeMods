modded class JMPlayerForm
{
	private UIActionButton m_TerjeRefreshStats;
	private UIActionButton m_TerjeApplyStats;
	private UIActionSlider m_TerjeUnused;
	private Widget m_TerjePerkTooltipWidget;
	private ref map<string, Widget> m_TerjePerkWidgets;
	private Widget m_TerjeStatsPanel;
	private Widget m_TerjePerksPanel;
	
	private ref map<string, UIActionSlider> m_TerjeControls;
	private ref set<string> m_TerjeUpdatedControls;

	override void OnInit()
	{
		// COT creates tabs lazily, but calls RefreshStats during its own OnInit.
		m_TerjeControls = new map<string, UIActionSlider>;
		m_TerjeUpdatedControls = new set<string>;
		super.OnInit();

		AddTab("Terje", JMConstants.Lucide("activity"), "TerjeBuildStatsTab");
#ifdef TERJE_SKILLS_MOD
		AddTab("Perks", JMConstants.Lucide("star"), "TerjeBuildPerksTab");
#endif
	}

	void TerjeBuildStatsTab(Widget uam_ParentPanel)
	{
		if (!uam_ParentPanel)
			return;

		m_TerjeStatsPanel = UIActionManager.CreateGridSpacer(uam_ParentPanel, 20, 2);
		if (!m_TerjeStatsPanel)
			return;
		Widget actions = m_TerjeStatsPanel;

		array<ref TerjeAdmintoolSupport_PlayerStat> registeredStats = new array<ref TerjeAdmintoolSupport_PlayerStat>;
		GetTerjeAdmintoolSupport().GetPlayerStats(registeredStats);
		foreach (ref TerjeAdmintoolSupport_PlayerStat stat : registeredStats)
		{
			m_TerjeControls[stat.GetId()] = UIActionManager.CreateSlider( actions, stat.GetName(), stat.GetMin(), stat.GetMax(), this, "Click_SetTerjeStat" );
			m_TerjeControls[stat.GetId()].SetSliderWidth(0.5);
			BindPermission(m_TerjeControls[stat.GetId()], "Admin.Player.Set.Health");
		}
		if (m_TerjeControls.Count() % 2 != 0)
		{
			m_TerjeUnused = UIActionManager.CreateSlider( actions, "[Unused]", 0, 1, this, "Click_SetTerjeUnused" );
			m_TerjeUnused.SetSliderWidth(0.5);
			m_TerjeUnused.SetEnabled(false);
		}
		m_TerjeApplyStats = UIActionManager.CreateButton( actions, "#STR_COT_PLAYER_MODULE_RIGHT_PLAYER_VARIABLES_APPLY", this, "Click_ApplyStats" );
		m_TerjeRefreshStats = UIActionManager.CreateButton( actions, "#STR_COT_PLAYER_MODULE_RIGHT_PLAYER_VARIABLES_REFRESH", this, "Click_RefreshTerjeStats" );

		BindPermission(m_TerjeApplyStats, "Admin.Player.Set.Health");
		RefreshStats(true);
	}

#ifdef TERJE_SKILLS_MOD
	void TerjeBuildPerksTab(Widget uam_ParentPanel)
	{
		if (!uam_ParentPanel)
			return;

		m_TerjePerkWidgets = new map<string, Widget>;
		array<ref TerjeSkillCfg> skills = new array<ref TerjeSkillCfg>;
		GetTerjeSkillsRegistry().GetSkills(skills);
		const int MAX_PERKS_PER_ROW = 11;
		const string TERJE_GRID_LAYOUT = "TerjeCompatibilityCOT/GUI/Layouts/GridSpacer11.layout";

<<<<<<< Updated upstream
		m_TerjePerksPanel = UIActionManager.CreateGridSpacer(uam_ParentPanel, 20, 1);
=======
		m_TerjePerksPanel = g_Game.GetWorkspace().CreateWidgets("TerjeCompatibilityCOT/GUI/Layouts/SkillsList.layout", uam_ParentPanel);
>>>>>>> Stashed changes
		if (!m_TerjePerksPanel)
			return;

		foreach (ref TerjeSkillCfg skillCfg : skills)
		{
			Widget skillBlock = UIActionManager.CreateGridSpacer( m_TerjePerksPanel, 4, 1 );
			UIActionManager.CreateText( skillBlock, skillCfg.GetDisplayName(), "" );
			array<ref TerjePerkCfg> perks = new array<ref TerjePerkCfg>;
			skillCfg.GetPerks(perks);
			array<ref TerjePerkCfg> visiblePerks = new array<ref TerjePerkCfg>;
			foreach (ref TerjePerkCfg perkCfgFilter : perks)
			{
				if (perkCfgFilter) visiblePerks.Insert(perkCfgFilter);
			}
			int perkCount = visiblePerks.Count();
			if (perkCount < 1) continue;
			int offset = 0;
			while (offset < perkCount)
			{
				int chunkSize = perkCount - offset;
				if (chunkSize > MAX_PERKS_PER_ROW) chunkSize = MAX_PERKS_PER_ROW;
				GridSpacerWidget perkRow = GridSpacerWidget.Cast(GetGame().GetWorkspace().CreateWidgets(TERJE_GRID_LAYOUT,skillBlock));
				for (int j = offset; j < offset + chunkSize; j++)
				{
					ref TerjePerkCfg perkCfgMain = visiblePerks.Get(j);
					Widget perkWidget = GetGame().GetWorkspace().CreateWidgets("TerjeSkills/Layouts/TerjePerkLayout.layout", perkRow);
					if (perkWidget)
					{
						string compositeId = skillCfg.GetId() + ":" + perkCfgMain.GetId();
						perkWidget.SetName(compositeId);
						perkWidget.FindAnyWidget("terje_perk_highlight").Show(false);
						WidgetEventHandler.GetInstance().RegisterOnMouseEnter(perkWidget, this, "OnQuickPerkEnter");
						WidgetEventHandler.GetInstance().RegisterOnMouseLeave(perkWidget, this, "OnQuickPerkLeave");
						WidgetEventHandler.GetInstance().RegisterOnMouseButtonDown(perkWidget, this, "OnMouseDown");
						WidgetEventHandler.GetInstance().RegisterOnMouseButtonUp(perkWidget, this, "OnMouseUp");
								
						m_TerjePerkWidgets.Insert(compositeId, perkWidget);
					}
				}
				offset = offset + chunkSize;
			}
		}

		UpdateTerjePerkWidgets();
	}

	// Quick perk tooltip handlers
	bool OnQuickPerkEnter(Widget w, int x, int y)
	{
		if (!w)
			return false;

		ItemManager itemManager = ItemManager.GetInstance();

		TerjeSkillCfg skillCfg;
		TerjePerkCfg perkCfg;
		string compositeId = w.GetName();
		if (compositeId == "" && w.GetParent())
			compositeId = w.GetParent().GetName();
		array<string> idParts = new array<string>;
		compositeId.Split(":", idParts);
		if (idParts.Count() < 2)
			return false;
		string skillId = idParts.Get(0);
		string perkId = idParts.Get(1);
		if (!GetTerjeSkillsRegistry().FindSkill(skillId, skillCfg))
			return false;
		if (!skillCfg.FindPerk(perkId, perkCfg))
			return false;

		int perkLevel = 0;
		int perkActiveLevel = 0;
		bool canBeUpgraded = false;

		if (m_SelectedInstance)
			m_SelectedInstance.GetTerjePerkStatus(skillCfg.GetId(), perkCfg.GetId(), perkLevel, perkActiveLevel, canBeUpgraded);

		// Remove any prior tooltip instance.
		if (m_TerjePerkTooltipWidget)
		{
			m_TerjePerkTooltipWidget.Unlink();
			m_TerjePerkTooltipWidget = null;
		}
		int mxi, myi;
		GetMousePos(mxi, myi);

		// Container with visible text header plus real Terje tooltip as child.
		Widget tooltip = GetGame().GetWorkspace().CreateWidget(FrameWidgetTypeID, mxi, myi, 420, 220, WidgetFlags.EXACTPOS | WidgetFlags.EXACTSIZE | WidgetFlags.IGNOREPOINTER | WidgetFlags.RENDER_ALWAYS, 0, 0, null);
		if (!tooltip) return false;

		Widget innerTooltip = TerjePerkTooltip.GetPerkTooltipWidget(perkCfg, perkLevel, perkActiveLevel, 0);
		if (innerTooltip)
		{
			
			tooltip.AddChild(innerTooltip);
			float iw, ih;
			innerTooltip.GetSize(iw, ih);
			innerTooltip.Show(true);
		}
		tooltip.Show(true);
		m_TerjePerkTooltipWidget = tooltip;
		return true;
	}

	bool OnQuickPerkLeave(Widget w, Widget enterW, int x, int y)
	{
		ItemManager itemManager = ItemManager.GetInstance();
		bool handled = false;
		if (itemManager)
		{
			itemManager.HideTooltip();
			handled = true;
		}
		if (m_TerjePerkTooltipWidget)
		{
			m_TerjePerkTooltipWidget.Unlink();
			m_TerjePerkTooltipWidget = null;
			handled = true;
		}
		return handled;
	}

	void OnMouseDown( Widget w, int x, int y, int button )
	{
		if (w)
		{
			Widget highlightWidget = w.FindAnyWidget("terje_perk_highlight");
			if (highlightWidget)
			{
				highlightWidget.SetColor(ARGB(120, 0, 0, 0));
			}
		}
	}

	void OnMouseUp( Widget w, int x, int y, int button )
	{
		if (w)
		{
			string composedId = w.GetName();
			array<string> idParts = new array<string>;
			composedId.Split(":", idParts);
			if (idParts.Count() < 2)
				return;
			string skillId = idParts.Get(0);
			string perkId = idParts.Get(1);
			PlayerBase player = null;

			if (m_SelectedInstance)
				player = PlayerBase.Cast(m_SelectedInstance.PlayerObject);

			if (!player)
<<<<<<< Updated upstream
=======
				player = PlayerBase.Cast(g_Game.GetPlayer());
			if (!player)
>>>>>>> Stashed changes
				return;
				
			if (button == MouseState.LEFT)
				player.TerjeRPCSingleParam(TerjeSkillsConstants.TRPC_PLAYER_PERK_APPLY, new Param2<string, string>(skillId, perkId), true, player.GetIdentity());
			else if (button == MouseState.RIGHT)
				player.TerjeRPCSingleParam(TerjeSkillsConstants.TRPC_PLAYER_PERK_DEC, new Param2<string, string>(skillId, perkId), true, player.GetIdentity());
						
			Widget highlightWidget = w.FindAnyWidget("terje_perk_highlight");
			if (highlightWidget)
			{
				highlightWidget.SetColor(ARGB(0, 0, 0, 0));
			}
		}
	}

	private void UpdateTerjePerkWidgets()
	{
		if (!m_TerjePerkWidgets)
			return;

		if (!m_SelectedInstance)
			return;

		foreach (string perkId, Widget perkWidget : m_TerjePerkWidgets)
		{
			if (!perkWidget)
				continue;

			array<string> idParts = new array<string>;
			perkId.Split(":", idParts);
			if (idParts.Count() < 2)
				continue;
			string skillId = idParts.Get(0);
			string perkOnlyId = idParts.Get(1);

			TerjeSkillCfg widgetSkillCfg;
			if (!GetTerjeSkillsRegistry().FindSkill(skillId, widgetSkillCfg))
				continue;

			TerjePerkCfg perkCfg;
			if (!widgetSkillCfg.FindPerk(perkOnlyId, perkCfg))
				continue;

			bool canBeUpgraded = false;
			int perkLevel = 0;
			int perkActiveLevel = 0;
			string iconPath = perkCfg.GetDisabledIcon();
			string levelPath = "set:TerjePerkLevels_icon image:L_NONE";
			string borderPath = "set:TerjeSkillBorders_icon image:ts_ui_perk_border0";
			
			m_SelectedInstance.GetTerjePerkStatus(widgetSkillCfg.GetId(), perkCfg.GetId(), perkLevel, perkActiveLevel, canBeUpgraded);

			if (perkLevel > 0)
			{
				if (perkCfg.GetStagesCount() == perkLevel)
				{
					levelPath = "set:TerjePerkLevels_icon image:L_MAX";
				}
				else
				{
					int perkClampedLevel = perkLevel;
					if (perkClampedLevel > 10)
						perkClampedLevel = 10;
					levelPath = "set:TerjePerkLevels_icon image:L" + perkClampedLevel.ToString();
				}
			}

			if (perkActiveLevel > 0)
			{
				iconPath = perkCfg.GetEnabledIcon();
			}
			else
			{
				iconPath = perkCfg.GetDisabledIcon();
			}

			if (perkActiveLevel == perkLevel && perkLevel > 0 && canBeUpgraded)
			{
				borderPath = "set:TerjeSkillBorders_icon image:ts_ui_perk_border3";
			}
			else if (canBeUpgraded)
			{
				borderPath = "set:TerjeSkillBorders_icon image:ts_ui_perk_border1";
			}
			else if (perkActiveLevel == perkLevel && perkLevel > 0)
			{
				borderPath = "set:TerjeSkillBorders_icon image:ts_ui_perk_border2";
			}
			else
			{
				borderPath = "set:TerjeSkillBorders_icon image:ts_ui_perk_border0";
			}

			ImageWidget.Cast(perkWidget.FindAnyWidget("terje_perk_icon")).LoadImageFile(0, iconPath);
			ImageWidget.Cast(perkWidget.FindAnyWidget("terje_perk_level")).LoadImageFile(0, levelPath);
			ImageWidget.Cast(perkWidget.FindAnyWidget("terje_perk_border")).LoadImageFile(0, borderPath);
			perkWidget.FindAnyWidget("terje_perk_highlight").Show(canBeUpgraded);
		}
	}
#endif

	protected override void OnTabUnfocus(int tab)
	{
		super.OnTabUnfocus(tab);
		TerjeHidePerkTooltip();
	}

	override void OnHide()
	{
		TerjeHidePerkTooltip();
		super.OnHide();
	}

	private void TerjeHidePerkTooltip()
	{
		if (m_TerjePerkTooltipWidget)
		{
			m_TerjePerkTooltipWidget.Unlink();
			m_TerjePerkTooltipWidget = null;
		}
	}

	override void RefreshStats(bool force = false)
	{
		super.RefreshStats(force);
		
		if (!m_SelectedInstance || !m_TerjeControls || !m_TerjeUpdatedControls)
			return;
		
		if (g_Game.IsClient() && m_SelectedInstance.GetDataLastUpdatedTime() < m_LastChangeTime)
			return;
		
		if (force)
		{
			m_TerjeUpdatedControls.Clear();
		}
		
		foreach (string id, UIActionSlider control : m_TerjeControls)
		{
			if (control && m_TerjeUpdatedControls.Find(id) == -1)
			{
				control.SetCurrent( m_SelectedInstance.GetTerjeStat(id) );
			}
		}

#ifdef TERJE_SKILLS_MOD
		// Refresh perk widgets
		UpdateTerjePerkWidgets();
#endif

	}
	
	void Click_RefreshTerjeStats( UIEvent eid, UIActionBase action )
	{
		if (eid != UIEvent.CLICK)
			return;

		RefreshStats(true);
	}

	// Called with the same confirmed targets as COT's vanilla stats.
	void TerjeApplyStats(TStringArray uam_Guids)
	{
		if (!m_TerjeUpdatedControls || !m_TerjeControls || !m_Module || !uam_Guids)
			return;

		foreach (string uam_StatId : m_TerjeUpdatedControls)
		{
			UIActionSlider uam_Control;
			if (m_TerjeControls.Find(uam_StatId, uam_Control) && uam_Control)
			{
				m_Module.SetTerjeValue(uam_StatId, uam_Control.GetCurrent(), uam_Guids);
			}
		}
		
		m_TerjeUpdatedControls.Clear();
	}
	
	void Click_SetTerjeStat( UIEvent eid, UIActionBase action )
	{
		if ( eid != UIEvent.CHANGE )
			return;

		UpdateLastChangeTime();
		
		string resultedId = "";
		foreach (string id, UIActionSlider control : m_TerjeControls)
		{
			if (control && control == action)
			{
				resultedId = id;
				break;
			}
		}
		
		if (resultedId != "")
		{
			m_TerjeUpdatedControls.Insert(resultedId);
		}
	}
	
	void Click_SetTerjeUnused( UIEvent eid, UIActionBase action )
	{

	}
}