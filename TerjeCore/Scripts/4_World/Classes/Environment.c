modded class Environment
{
	override void ProcessHeatComfort()
	{
		if (m_Player && m_Player.GetTerjeDisableHeatComfort())
		{
			m_HeatComfort = 0.0;
 			SetTargetHeatComfort(0.0);
 			m_Player.GetStatHeatComfort().Set(0.0);
#ifdef DAYZ_1_30
			// DayZ 1.30 tracks accumulated overheating in ThermalBias separately from Heat Comfort.
			// Setting Heat Comfort to zero alone leaves the previously accumulated ThermalBias,
			// so HeatStroke modifiers can remain active until that value decays.
			// While Terje DisableHeatComfort is active, clear this additional state as well,
			// allowing the modifiers to deactivate on their next condition check.
			// Normal thermal processing still delegates to super when the flag is inactive.
			// The version guard keeps the absent ThermalBiasHandler out of the 1.29 build.
			ThermalBiasHandler uam_ThermalBiasHandler = m_Player.GetThermalBiasHandler();
			if (uam_ThermalBiasHandler)
				uam_ThermalBiasHandler.Set(0.0);
#endif
			return;
		}
		
		super.ProcessHeatComfort();
	}
	
	override void SetHeatcomfortDirectly()
	{
		if (m_Player && m_Player.GetTerjeDisableHeatComfort())
		{
			m_HeatComfort = 0.0;
 			SetTargetHeatComfort(0.0);
 			m_Player.GetStatHeatComfort().Set(0.0);
#ifdef DAYZ_1_30
			// DayZ 1.30 tracks accumulated overheating in ThermalBias separately from Heat Comfort.
			// Setting Heat Comfort to zero alone leaves the previously accumulated ThermalBias,
			// so HeatStroke modifiers can remain active until that value decays.
			// While Terje DisableHeatComfort is active, clear this additional state as well,
			// allowing the modifiers to deactivate on their next condition check.
			// Normal thermal processing still delegates to super when the flag is inactive.
			// The version guard keeps the absent ThermalBiasHandler out of the 1.29 build.
			ThermalBiasHandler uam_ThermalBiasHandler = m_Player.GetThermalBiasHandler();
			if (uam_ThermalBiasHandler)
				uam_ThermalBiasHandler.Set(0.0);
#endif
			return;
		}
		
		super.SetHeatcomfortDirectly();
	}
}