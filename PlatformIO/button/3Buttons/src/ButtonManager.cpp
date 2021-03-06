#include "ButtonManager.h"

volatile uint32_t g_button1DebounceTime = 0; //guarda o tempo de debounce
volatile uint32_t g_button2DebounceTime = 0; //guarda o tempo de debounce
volatile uint32_t g_button3DebounceTime = 0; //guarda o tempo de debounce
volatile uint32_t g_button1PressedTime = 0; //guarda o tempo de debounce
volatile uint32_t g_button2PressedTime = 0; //guarda o tempo de debounce
volatile uint32_t g_button3PressedTime = 0; //guarda o tempo de debounce

ButtonSettings_t g_settings;

/**
 * Function: OnButton1Pressed
 * Description: Interruption raised by the Button 1 press. 
 */
void IRAM_ATTR OnButton1Pressed()
{
    uint32_t currentTime = millis();
    bool currentState = digitalRead(g_settings.Button1Pin); 

    if ( (currentTime - g_button1DebounceTime) > DEBOUNCE_WAIT_TIME )
    {
        if (currentState)
        {
            ESP_LOGI(TAG_BUTTONMANAGER, "Botao 1 solto");
            digitalWrite(g_settings.Button1Led, LOW);
            ButtonEventData_t eventData;
            eventData.PressedTime = currentTime - g_button1PressedTime;
            eventData.Source = BUTTON_1;
            ButtonManager.notify(eventData);
        }
        else
        {
            ESP_LOGI(TAG_BUTTONMANAGER, "Botao 1 pressionado");
            digitalWrite(g_settings.Button1Led, HIGH);
            g_button1PressedTime = currentTime;
        }
        
        g_button1DebounceTime = currentTime;
    }
}

/**
 * Function: OnButton2Pressed
 * Description: Interruption raised by the Button 2 press. 
 */
void IRAM_ATTR OnButton2Pressed()
{
    uint32_t currentTime = millis();
    bool currentState = digitalRead(g_settings.Button2Pin); 

    if ( (currentTime - g_button2DebounceTime) > DEBOUNCE_WAIT_TIME )
    {
        if (currentState)
        {
            ESP_LOGI(TAG_BUTTONMANAGER, "Botao 2 solto");
            digitalWrite(g_settings.Button2Led, LOW);
            ButtonEventData_t eventData;
            eventData.PressedTime = currentTime - g_button2PressedTime;
            eventData.Source = BUTTON_2;
            ButtonManager.notify(eventData);
        }
        else
        {
            ESP_LOGI(TAG_BUTTONMANAGER, "Botao 2 pressionado");
            digitalWrite(g_settings.Button2Led, HIGH);
            g_button2PressedTime = currentTime;
        }

        g_button2DebounceTime = currentTime;
    }
}

/**
 * Function: OnButton3Pressed
 * Description: Interruption raised by the Button 3 press. 
 */
void IRAM_ATTR OnButton3Pressed()
{
    uint32_t currentTime = millis();
    bool currentState = digitalRead(g_settings.Button3Pin);

    if ( (currentTime - g_button3DebounceTime) > DEBOUNCE_WAIT_TIME )
    {
        if (currentState)
        {
            ESP_LOGI(TAG_BUTTONMANAGER, "Botao 3 solto");
            digitalWrite(g_settings.Button3Led, LOW);
            ButtonEventData_t eventData;
            eventData.PressedTime = currentTime - g_button3PressedTime;
            eventData.Source = BUTTON_3;
            ButtonManager.notify(eventData);
        }
        else
        {
            ESP_LOGI(TAG_BUTTONMANAGER, "Botao 3 pressionado");
            digitalWrite(g_settings.Button3Led, HIGH);
            g_button3PressedTime = currentTime;
        }
        g_button3DebounceTime = currentTime;
    }
}

/**
 * Method: ButtonManagerClass
 * Description: ButtonManagerClass Constructor. 
 */
ButtonManagerClass::ButtonManagerClass()
{

}

/**
 * Method: begin
 * Description: Initializes the ButtonManagerClass. 
 * Returns: esp_err_t ESP_OK
 */
esp_err_t ButtonManagerClass::begin(ButtonSettings_t settings)
{
    // #define LOGI_0010 "Inicializando o gerenciador de botoes."
    ESP_LOGI(TAG_BUTTONMANAGER, LOGI_0010);

    g_settings.Button1Pin = settings.Button1Pin;
    g_settings.Button2Pin = settings.Button2Pin;
    g_settings.Button3Pin = settings.Button3Pin;
    g_settings.Button1Led = settings.Button1Led;
    g_settings.Button2Led = settings.Button2Led;
    g_settings.Button3Led = settings.Button3Led;    

    pinMode(g_settings.Button1Led, OUTPUT);
    pinMode(g_settings.Button2Led, OUTPUT);
    pinMode(g_settings.Button3Led, OUTPUT);

    digitalWrite(g_settings.Button1Led, LOW);
    digitalWrite(g_settings.Button2Led, LOW);
    digitalWrite(g_settings.Button3Led, LOW);

    pinMode(g_settings.Button1Pin, INPUT_PULLUP);
    pinMode(g_settings.Button2Pin, INPUT_PULLUP);
    pinMode(g_settings.Button3Pin, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(g_settings.Button1Pin), OnButton1Pressed, CHANGE);
    attachInterrupt(digitalPinToInterrupt(g_settings.Button2Pin), OnButton2Pressed, CHANGE);
    attachInterrupt(digitalPinToInterrupt(g_settings.Button3Pin), OnButton3Pressed, CHANGE);

    // #define LOGI_0011 "Gerenciador de botoes inicializado."
    ESP_LOGI(TAG_BUTTONMANAGER, LOGI_0011);
    return ESP_OK;
}

/**
 * Method: attach
 * Description: Attach an observer to the class events. 
 * Parameters:
 *  -> IButtonObserver *observer: object that will receive the ButtonManagerClass updates.
 * Returns: void
 */
void ButtonManagerClass::attach(IButtonObserver *observer)
{
    m_list.push_back(observer);
}

/**
 * Method: dettach
 * Description: Detach an observer of the class events. 
 * Parameters:
 *  -> IButtonObserver *observer: object that will be detached.
 * Returns: void
 */
void ButtonManagerClass::detach(IButtonObserver *observer)
{
    m_list.erase(std::remove(m_list.begin(), m_list.end(), observer), m_list.end());
}

/**
 * Method: notify
 * Description: Notifies the observers about a new event. 
 * Parameters:
 *  -> ButtonEventData_t eventData: the data generated by the event.
 * Returns: void
 */
void ButtonManagerClass::notify(ButtonEventData_t eventData)
{
    for(std::vector<IButtonObserver*>::const_iterator iter = m_list.begin(); iter != m_list.end(); ++iter)
    {
        if (*iter != 0)
        {
            (*iter)->OnButtonPressed(eventData);
        }
    }
}

ButtonManagerClass ButtonManager;