/************************************************************
@Inputs:
@Outputs:
@Comments:
*************************************************************/
void Start_wifi(wifi_config_t* wifi_config , wifi_mode_t Mode )
{
   wifi_mode_t currentMode;
   
   if(esp_wifi_get_mode(&currentMode) == ESP_ERR_WIFI_NOT_INIT)
   {
      wifiSetup_Init();
   }
   else if(currentMode == WIFI_MODE_APSTA)
   {
      ESP_LOGE(TAG,"wifi APSTA already running\n");
      return;
   }
   else if(currentMode == Mode)
   {
      ESP_LOGE(TAG,"that mode is already running\n");
      return;
   }
   else
   {
      esp_wifi_stop();
   }

      if(currentMode == WIFI_MODE_AP && Mode == WIFI_MODE_STA)
      {
         ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_APSTA) );
      }
      else if(currentMode == WIFI_MODE_STA && Mode == WIFI_MODE_AP)
      {
         ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_APSTA) );
      }
      else
      {
         ESP_ERROR_CHECK( esp_wifi_set_mode(Mode) );
      }
   if(Mode == WIFI_MODE_AP)
   {
      ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_AP, wifi_config) );
   }
   else if(Mode == WIFI_MODE_STA)
   {
      ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config) );
   }
   ESP_ERROR_CHECK( esp_wifi_start());
}
/************************************************************
@Inputs:
@Outputs:
@Comments:
*************************************************************/
void wifiSetup_StopSTA(void)
{
   wifi_mode_t currentMode;
   esp_wifi_get_mode(&currentMode);
   if(currentMode == WIFI_MODE_STA )
   {
      esp_wifi_stop();
      ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL) );
   }
   else if (currentMode == WIFI_MODE_APSTA)
   {
      ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );
   }
}
/************************************************************
@Inputs:
@Outputs:
@Comments:
*************************************************************/
void wifiSetup_StopAP(void)
{
   wifi_mode_t currentMode;
   esp_wifi_get_mode(&currentMode);
   if(currentMode == WIFI_MODE_AP )
   {
      esp_wifi_stop();
      ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL) );
   }
   else if (currentMode == WIFI_MODE_APSTA)
   {
      ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
   }
}
/************************************************************
@Inputs:
@Outputs:
@Comments:
*************************************************************/
void Kill_wifi(void)
{

   esp_wifi_stop();
   //vEventGroupDelete(wifi_event_group);
   esp_wifi_deinit();
}

/************************************************************
@Inputs:
@Outputs:
@Comments:
*************************************************************/
void wifiSetup_Init(void)
{
   ESP_LOGI(TAG,"Initilising TCP and wifi\n");
   tcpip_adapter_init();
   wifi_event_group = xEventGroupCreate();
   if(esp_event_loop_init(event_handler, NULL) == ESP_FAIL)
   {
         ESP_LOGI(TAG,"Event Loop not created , may have been created before \n");
   }
   wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
   ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
   ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
   esp_wifi_set_mode(0);
}