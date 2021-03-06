//---------DODAJCIE BIBLIOTEKE #include "stdio.h" ------------

/* USER CODE BEGIN 0 */
	uint8_t enginesFrame[5];		// Ramka, do ktorej UART wpisuje odbierane dane dot. silnikow
	struct EnginesData{				// Struktura przechowujaca skonwertowane dane dla silnikow
		int multiplier;
		int leftPower, rightPower;
		int leftDirection, rightDirection;
	}enginesData;
/* USER CODE END 0 */

/* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart1, enginesFrame, 6);	// Inicjalizacja odbierania danych przez UART w trybie przerwaniowym
/* USER CODE END 2 */

/* USER CODE BEGIN 4 */
void convertEnginesFrame(){		// Konwertuje dane do struktury dla silnikow

	char buffer[2];

	sprintf(buffer, "%c", enginesFrame[0]);		// Przepisanie z uint8_t na char (nie wiem czemu funkcja atoi nie chce konwertowac uint8_t)
	enginesData.multiplier = atoi(buffer);		// Konwersja i przypisanie wartosci z ramki
												// Ponizej dokladnie to samo dla reszty zmiennych
	sprintf(buffer, "%c", enginesFrame[1]);
	enginesData.leftPower = atoi(buffer);

	sprintf(buffer, "%c", enginesFrame[2]);
	enginesData.rightPower = atoi(buffer);

	sprintf(buffer, "%c", enginesFrame[3]);
	enginesData.leftDirection = atoi(buffer);

	sprintf(buffer, "%c", enginesFrame[4]);
	enginesData.rightDirection = atoi(buffer);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){	// Funkcja wykonywana, gdy USART odbierze dane i wywola przerwanie

	uint8_t wiadomosc[11] = "Odebrano\n\r";

	if (huart->Instance == USART1){		// Sprawdzenie czy wyslano przez USART1, nie trzeba tego uzywac dopoki uzywamy tylko jednego modulu USART
		HAL_UART_Transmit_IT(&huart1, wiadomosc, 11);	// Wyslanie potwierdzenia odebrania danych
		HAL_GPIO_TogglePin(LED_Blue_GPIO_Port, LED_Blue_Pin); // Zmiana stanu pinu na diodzie LED
		convertEnginesFrame();		// Wywolanie funkcji adresuj¹cej odebrane dane do struktury
		HAL_UART_Receive_IT(&huart1, enginesFrame, 6);
	}
}
/* USER CODE END 4 */
