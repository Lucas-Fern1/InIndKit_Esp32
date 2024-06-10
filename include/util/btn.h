#include <Arduino.h>

#define DEBOUNCETIME 50

class Btn_c
{
protected:
    unsigned long reading_time = 0;
    bool status_btn = false;
    bool last_status_btn = false;

public:
    void (*onChanged)(bool status) = NULL;
    uint8_t pin;
    Btn_c() {pin = 0;}
    Btn_c(uint8_t pinBtn) {pin = pinBtn;}
    void onValueChanged(void (*f)(bool status));
    bool debounceBtn();
    void setPin(uint8_t pinBtn);
    void update();
    bool getStatusBtn();
};

void Btn_c::setPin(uint8_t pinBtn)
{
    pin = pinBtn;
}

void Btn_c::update()
{
    if (debounceBtn())
        onChanged(status_btn);
}

bool Btn_c::debounceBtn()
{
    bool leitura = (bool)digitalRead(pin); // A variável leitura recebe a leitura do pino do botão: HIGH (pressionado) ou LOW (Desacionado)
    if (leitura != last_status_btn)        // Se a leitura atual for diferente da leitura anterior
    {
        reading_time = millis();   // Reseta a variável btnData.reading_time atribuindo o tempo atual para uma nova contagem
        last_status_btn = leitura; // Atualiza a variável ultimoStatusBotao para o que foi lido na variável leitura
    }
    if ((millis() - reading_time) > DEBOUNCETIME) // Se o resultado de (tempo atual - tempoUltimoDebounce) for maior que o tempo que determinamos (tempoDebounce), ou seja, já passou os 50 milissegundos que é o tempo que o botão precisa ficar pressionado para ter a certeza de que ele realmente foi pressionado? Se sim faça:
    {
        if (leitura != status_btn)
        {                         // Verifica se a leitura do botão mudou, ou seja, se é diferente do status que o botão tinha da última vez. Se sim, faça:
            status_btn = leitura; // statusBotao recebe o que foi lido na variável leitura (pressionado = 1 e não pressionado = 0)
            return (true);        // Se valor do status foi alterado
        }
    }
    return (false);
}

bool Btn_c::getStatusBtn()
{
    return status_btn;
}
void Btn_c::onValueChanged(void (*f)(bool status))
{
    onChanged = f;
}