

/**<    ***** TRABAJO PRACTICO NUMERO 2*****
        ***** CERRADURA *****
        ***** Integrantes: Hugo Miranda, German Molinari, Gaston Rando, Lucas Ramirez *****
 */



#include <LiquidCrystal.h>
#include <Keypad.h>
#include <string.h>

#define FILAS 4
#define COLUMNAS 4
#define LED_VERDE 9
#define LED_AMARILLO 10
#define LED_ROJO 11

//VARIABLES----------------------------------
int contadorArranque = 0;
int banderaConfig =0;
int indiceCon =0;
int banderaParpadeoLed = 0;
int banderaVerificacion = 0;
int banderaFinalLed = 0;
int banderaTest = 0;
int banderaReset = 0;
int banderaPrimerConfig = 0;
int banderaClear = 0;
int banderaSegConfig = 0;



//VARIABLES DE TIEMPO--------------------------
int periodo = 1000;
unsigned long millisTotales = 0;
unsigned long millisAnterior = 0;


//VARIABLES TECLADO----------------------------

char teclas[FILAS][COLUMNAS] =
{
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

byte pinesFilas[FILAS] = {7,6,5,4};
byte pinesColumnas[COLUMNAS] = {3,2,1,0};

Keypad teclado = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, byte(FILAS), byte(COLUMNAS));

int teclasPulsadas = 0;
int indice = 0;
int indiceConfirm =0;
char teclaPresionada;
char clave[8];
char claveOriginal[8] = "2C2021";
char claveConfig [8];
char claveConfigConfirm [8];
char cabezalLcd[13] = "PASSWORD:";


//SETUP PANTALLA-----------------------------------
LiquidCrystal lcd(8, 12, A0, A1, A2, A3);

//VARIABLES LEDS-----------------------------------
unsigned long millisActual = 0;

int estadoLedVerde = LOW;
unsigned long millisAnteriorVerde = 0;

int estadoLedAmarillo = LOW;
unsigned long millisAnteriorAmarillo = 0;

int estadoLedRojo = LOW;
unsigned long millisAnteriorRojo = 0;

int tiempoPasado = 0;

// PROTOTIPOS DE FUNCIONES ----------------------

void inicializarDisplay();
/** \brief Limpia pantalla y nos pone el menu de la contraseña.
 *
 * \return void
 *
 */

int leerBoton(int lectura);
/** \brief lee el boton retornando los botones segun su tension.
 *
 * \param lectura int
 * \return int
 *
 */

int validarInput(char clave[], int* indice, char teclaPresionada);
/** \brief //Validamos el tamaño de la contraseña
 *
 * \param clave[] char
 * \param indice int*
 * \param teclaPresionada char
 * \return int
 *
 */

int validarPassword(char clave[], int* indice, char claveOriginal[]);
/** \brief validamos si la contraseña ingresada es correcta
 *
 * \param clave[] char
 * \param indice int*
 * \param claveOriginal[] char
 * \return int
 *
 */
int validarInputCon(char clave[], int* indiceCon, char teclaPresionada);
/** \brief validamos contraseña para que esta pueda ser cambiada despues
 *
 * \param clave[] char
 * \param indiceCon int*
 * \param teclaPresionada char
 * \return int
 *
 */
int prenderLed(int led, unsigned long *millisActual, unsigned long *millisAnterior, int intervalo, int *estadoLed,
               int tiempoTotal, int *bandera, unsigned long* millisTotales);
/** \brief Prende el led  lo apaga durante 5 segundos para el led rojo y verde
 *
 * \param
 * \param
 * \return
 *
 */

void prendeLedOK(int led, int* estadoLed);
/** \brief negamos el estado de led para usarlo en el amarillo
 *
 * \param led int
 * \param estadoLed int*
 * \return void
 *
 */

//------------------------------------------------


void setup()
{
    pinMode(LED_VERDE, OUTPUT);
    pinMode(LED_AMARILLO, OUTPUT);
    pinMode(LED_ROJO, OUTPUT);
    pinMode(A4, INPUT);

    lcd.begin(16, 2);
    //Serial.begin(9600); //ME ROMPE EL KEYPAD
    inicializarDisplay();
}

void loop()
{

    //Serial.println(lectura);  //testing de tension para c/ boton
    millisActual = millis();

    int boton = analogRead(A4);

    teclaPresionada = teclado.getKey();
    validarInput(clave, &indice, teclaPresionada);


    boton = leerBoton(boton);

    if(strcmp(clave, claveOriginal) == 0)
    {
        banderaVerificacion = 1;
    }

// Serial.println(boton); TESTING VALOR BOTON

    switch(boton)
    {

    case 1:

        banderaReset =1;

        break;

    case 2:

        banderaTest = 1;
        break;

    case 3:
        if(!banderaVerificacion)
        {
            lcd.clear();
            lcd.setCursor(0,0);
            banderaClear=0;
            lcd.print("NEED PASSWORD");
        }
        else
        {
            banderaConfig = 1;
        }
        break;
    }

    if(banderaReset ==1)
    {
        inicializarDisplay();
        delay(25);
        banderaReset =0;
        indice=0;
    }

    if (indice == 7 || banderaTest == 1 && banderaConfig !=1 )
    {
        if(validarPassword(clave, &indice, claveOriginal)==0)
        {
            banderaFinalLed=prenderLed(LED_VERDE,&millisActual, &millisAnteriorVerde, 300,&estadoLedVerde,5000, &banderaParpadeoLed, &millisTotales);
            if( banderaFinalLed == 1)
            {

                indice=0;
                inicializarDisplay();
                banderaTest=0;

            }

        }
        else
        {
            banderaFinalLed=prenderLed(LED_ROJO,&millisActual, &millisAnteriorRojo, 300,&estadoLedRojo,5000, &banderaParpadeoLed, &millisTotales);
            if( banderaFinalLed == 1)
            {

                indice=0;
                banderaTest=0;
                inicializarDisplay();

            }
        }
    }




    if(banderaConfig == 1)
    {
        if(banderaPrimerConfig==0)
        {

            lcd.setCursor(0,0);
            lcd.print("SET PASSWORD: ");
            lcd.setCursor(0,1);
            teclaPresionada = teclado.getKey();
            validarInput(clave, &indiceCon, teclaPresionada);
            strcpy(claveConfig,clave);
        }

        if(indiceCon == 7)
        {
            banderaPrimerConfig =1;
            //strcpy(claveConfig,clave);
            if(banderaSegConfig==0)
            {
                if(banderaClear == 0)
                {
                    lcd.clear();
                    banderaClear = 1;
                }
                lcd.setCursor(0,0);
                lcd.print("CONFIRM PASS:");//LO MUESTRA Y SALE RAPIDO att: Ger
                lcd.setCursor(0,1);
                teclaPresionada = teclado.getKey();
                validarInput(clave, &indiceConfirm, teclaPresionada);
            }

            if(indiceConfirm == 7)
            {
                banderaSegConfig=1;
                strcpy(claveConfigConfirm,clave);
                if(strcmp(claveConfig,claveConfigConfirm) == 0)
                {

                    strcpy(claveOriginal,clave);
                    if(banderaClear == 1)
                    {
                        lcd.clear();
                        banderaClear = 2;
                        millisTotales=millisActual+1000;
                    }
                    lcd.setCursor(0,0);
                    lcd.print("SUCCESS"); //TESTING
                    if(millisTotales<millisActual)
                    {
                        millisAnterior= millisActual;
                        lcd.setCursor(0,1);
                        banderaConfig=0;
                        indiceCon=0;
                        //indice=0;
                        banderaPrimerConfig=0;
                        inicializarDisplay();
                        banderaClear=0;
                        millisTotales=0;
                        indiceConfirm=0;
                        banderaSegConfig=0;


                    }

                }
                else
                {

                    if(banderaClear == 1)
                    {
                        lcd.clear();
                        banderaClear = 2;
                        millisTotales=millisActual+1000;
                    }
                    lcd.setCursor(0,0);
                    lcd.print("ERROR"); //TESTING
                    if(millisTotales<millisActual)
                    {
                        millisAnterior= millisActual;
                        lcd.setCursor(0,1);
                        banderaConfig=0;
                        indiceCon=0;
                        indice=0;
                        banderaPrimerConfig=0;
                        inicializarDisplay();
                        banderaClear=0;
                        millisTotales=0;
                        indiceConfirm=0;
                        banderaSegConfig=0;


                    }
                }

            }



        }
    }

    //primera validacion de contraseña
    if(contadorArranque == 0 && indice == 6 )
    {



        if(validarPassword(clave, &indice, claveOriginal)==0)
        {
            banderaFinalLed=prenderLed(LED_VERDE,&millisActual, &millisAnteriorVerde, 300,&estadoLedVerde,5000, &banderaParpadeoLed, &millisTotales);
            if( banderaFinalLed == 1)
            {
                contadorArranque=1;
                indice=0;
                inicializarDisplay();
                prendeLedOK(LED_AMARILLO, &estadoLedAmarillo);

            }

        }
        else
        {
            banderaFinalLed=prenderLed(LED_ROJO,&millisActual, &millisAnteriorRojo, 300,&estadoLedRojo,5000, &banderaParpadeoLed, &millisTotales);
            if( banderaFinalLed == 1)
            {
                //contadorArranque=1;
                indice=0;
                inicializarDisplay();

            }

        }


    }


}


void inicializarDisplay()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("PASSWORD: ");
    lcd.setCursor(0,1);
}

//lee la corriente de los botones configuracion, test pass y reset
int leerBoton(int lectura)
{
    int retorno = 0;

    //sin presionar boton 1023 - primer boton 512 - Segundo Boton 682 - Tercer boton 767


    if(lectura >502 && lectura <522)
    {
        retorno =  1;
    }

    if(lectura >672 && lectura <692)
    {
        retorno =  2;
    }

    if(lectura >757 && lectura <777)
    {
        retorno =  3;

    }
    return retorno;
}


int validarInput(char clave[], int* indice, char teclaPresionada)
{


    if(clave!=NULL && indice!=NULL)
    {

        if(teclaPresionada != NO_KEY)
        {

            lcd.setCursor(*indice,1);
            lcd.print(teclaPresionada);
            clave[(*indice)] = teclaPresionada;
            (*indice)++;

        }
    }
}

int validarInputCon(char clave[], int* indiceCon, char teclaPresionada)
{


    if(clave!=NULL && indiceCon!=NULL)
    {

        if(teclaPresionada != NO_KEY)
        {

            lcd.setCursor(*indiceCon,1);
            lcd.print(teclaPresionada);
            clave[(*indiceCon)] = teclaPresionada;
            (*indiceCon)++;

        }
    }
}

int validarPassword(char clave[], int* indice, char claveOriginal[])
{
    int retorno = -1;
    if(strcmp(clave, claveOriginal) == 0)
    {
        if(contadorArranque == 0)
        {
            //contadorArranque=1;
            lcd.setCursor(0,1);
            //lcd.print(contadorArranque);
        }

        //strcpy(cabezalLcd, "PASSWORD OK");

        lcd.setCursor(0,0);
        lcd.print("PASSWORD OK");

        retorno=0;

    }
    else //if (strcmp(clave, claveOriginal) != 0)
    {
        lcd.setCursor(0,0);
        lcd.print("PASSWORD FAIL");

        retorno=1;

    }
    return retorno;
}

//prendido y apagado de LEDs
int prenderLed(int led, unsigned long *millisActual, unsigned long *millisAnterior, int intervalo, int *estadoLed,
               int tiempoTotal, int *bandera, unsigned long* millisTotales)
{

    // lcd.print(millis()); //validacion millis

    int retorno=-1;
    if (*bandera==0)
    {

        *millisTotales = *millisActual+5000;
        *bandera = 1;

    }
    if (*millisActual<*millisTotales && *millisTotales>0)
    {
        retorno=0;
        //lcd.print(millis());

        if (*millisActual - *millisAnterior >= 300)
        {


            *millisAnterior = *millisActual;



            if (*estadoLed == LOW)
            {

                *estadoLed = HIGH;

            }
            else
            {

                *estadoLed = LOW;

            }

        }

        digitalWrite(led, *estadoLed);

    }
    else
    {
        *bandera=0;
        *millisTotales=0;
        //contadorArranque=1;
        //*indice=0;
        digitalWrite(led, LOW);
        retorno=1;

    }
    //lcd.print(retorno);
    //delay(50);
    return retorno;
}

void prendeLedOK(int led, int* estadoLed)
{

    digitalWrite(led, !(*estadoLed));

}
