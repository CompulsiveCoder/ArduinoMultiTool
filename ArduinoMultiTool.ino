#include <Wire.h>
#include <PCD8544.h>

long lastButtonReading = 0;
#define buttonReadingInterval 125

long lastDisplayRefresh = 0;
#define displayRefreshInterval 100

static PCD8544 display;

int isVerbose = 1;

#define upButtonPin 8
#define downButtonPin 11
#define leftButtonPin 10
#define rightButtonPin 12
#define selectButtonPin 9

#define buttonActiveValue 0

#define MENU_HOME 0
#define MENU_INPUT 1
#define MENU_INPUT_PIN 11
#define MENU_OUTPUT 2
#define MENU_OUTPUT_PIN 21

int currentMenuPosition = 0;
int currentMenuHoverPosition = 1;
int currentMenuItemCount = 2;
int currentMenuLevel = 1;
int currentMenuScrollOffset = 0;
int currentMenuPinSelection = 0;

#define MENU_ROWS 5

#define TOTAL_ANALOG_PINS 8
#define TOTAL_DIGITAL_PINS 14

void setup()
{
  Serial.begin(9600);

  Serial.println("Starting multitool");

  delay(500);

  display.begin(84, 84);

  display.clear();

  display.print("Multitool");

  delay(500);

  display.clear();
}

void loop()
{
  if (lastButtonReading + buttonReadingInterval < millis())
  {
    checkButtons();

    lastButtonReading = millis();
  }

  if (lastDisplayRefresh + displayRefreshInterval < millis())
  {
    render();

    lastDisplayRefresh = millis();
  }
}

void render()
{
  //Serial.println("Rendering");

  display.setCursor(0, 0);

  switch (currentMenuPosition)
  {
    case MENU_HOME:
      renderMenuHome();
      break;
    case MENU_INPUT:
      renderInputMenu();
      break;
    case MENU_INPUT_PIN:
      //      renderInputPin();
      break;
    case MENU_OUTPUT:
      renderOutputMenu();
      break;
    case MENU_OUTPUT_PIN:
      renderOutputPinMenu();
      break;
  }
}

void renderMenuHome()
{
  display.setCursor(0, 1);
  display.print(currentMenuHoverPosition == MENU_INPUT ? ">" : " ");
  display.print(" Input");
  display.setCursor(0, 2);
  display.print(currentMenuHoverPosition == MENU_OUTPUT ? ">" : " ");
  display.print(" Output");
}

void renderInputMenu()
{
  display.setCursor(0, 0);
  display.print("Input");

  int firstPositionVisible = currentMenuScrollOffset;
  int lastPositionVisible = MENU_ROWS + currentMenuScrollOffset;

  for (int i = 0; i < TOTAL_ANALOG_PINS; i++)
  {
    if (i >= firstPositionVisible
        && i <= lastPositionVisible)
    {
      int rowIndex = i + 1 - currentMenuScrollOffset;

      display.setCursor(0, rowIndex);
      display.print(currentMenuHoverPosition == i + 1 ? ">" : " ");
      display.print(" A");
      display.print(i);
    }
  }
}

void renderOutputMenu()
{
  display.setCursor(0, 0);
  display.print("Output");

  int firstPositionVisible = currentMenuScrollOffset;
  int lastPositionVisible = MENU_ROWS + currentMenuScrollOffset;

  for (int i = 0; i < TOTAL_DIGITAL_PINS; i++)
  {
    if (i >= firstPositionVisible
        && i <= lastPositionVisible)
    {
      int rowIndex = i + 1 - currentMenuScrollOffset;

      display.setCursor(0, rowIndex);
      display.print("         ");
      display.setCursor(0, rowIndex);
      display.print(currentMenuHoverPosition == i + 1 ? ">" : " ");
      display.print(" D");
      display.print(i);
    }

  }
}

void renderOutputPinMenu()
{
  display.setCursor(0, 0);
  display.print("Output: D");
  display.print(currentMenuPinSelection);

  display.setCursor(0, 1);
  display.print(currentMenuHoverPosition == MENU_INPUT ? ">" : " ");
  display.print(" LOW");
  display.setCursor(0, 2);
  display.print(currentMenuHoverPosition == MENU_OUTPUT ? ">" : " ");
  display.print(" HIGH");
}

void checkButtons()
{
  if (lastButtonReading + buttonReadingInterval < millis()
      || lastButtonReading == 0)
  {
    bool buttonPressed = false;

    if (digitalRead(upButtonPin) == buttonActiveValue)
    {
      upButton();
      buttonPressed = true;
    }

    if (digitalRead(downButtonPin) == buttonActiveValue)
    {
      downButton();
      buttonPressed = true;
    }

    if (digitalRead(leftButtonPin) == buttonActiveValue)
    {
      leftButton();
      buttonPressed = true;
    }

    if (digitalRead(rightButtonPin) == buttonActiveValue)
    {
      rightButton();
      buttonPressed = true;
    }

    if (digitalRead(selectButtonPin) == buttonActiveValue)
    {
      selectButton();
      buttonPressed = true;
    }

    if (buttonPressed)
      lastButtonReading = millis();
  }
}

void upButton()
{
  Serial.println("Up button");

  up();
}

void downButton()
{
  Serial.println("Down button");

  down();
}

void leftButton()
{
  Serial.println("Left button");

  back();
}

void rightButton()
{
  Serial.println("Right button");

  selectItem();
}

void selectButton()
{
  Serial.println("Select button");

  selectItem();
}

void selectItem()
{
  Serial.print("Selected item - Position: ");
  Serial.print(currentMenuHoverPosition);
  Serial.print("  Menu level: ");
  Serial.println(currentMenuLevel);

  switch (currentMenuPosition)
  {
    case MENU_HOME:
      selectItemFromHomeMenu();
      break;
    case MENU_OUTPUT:
      selectItemFromOutputMenu();
      break;
    case MENU_OUTPUT_PIN:
      selectItemFromOutputPinMenu();
      break;
    /*case MENU_INPUT:
      selectInputMenuItem();
      break;
    case MENU_INPUT_PIN:
      selectInputMenuPin();
      break;
    case MENU_OUTPUT:
      selectOutputMenuItem();
      break;
    case MENU_OUTPUT_PIN:
      selectOutputMenuPin();
      break;*/
    default:
      Serial.print("Unknown menu position: " + currentMenuPosition);
  }
}

void selectItemFromHomeMenu()
{
  switch (currentMenuHoverPosition)
  {
    case MENU_INPUT:
      selectInputMenuItem();
      break;
    case MENU_OUTPUT:
      selectOutputMenuItem();
      break;
  }
}

void selectInputMenuItem() {
  Serial.println("Selected 'Input'");

  inputMenu();
}

void selectOutputMenuItem()
{
  Serial.println("Selected 'Output'");

  outputMenu();
}

void selectItemFromOutputMenu()
{
  int p = currentMenuHoverPosition - 1;

  currentMenuPinSelection = p;

  Serial.print("Selected output pin: ");
  Serial.println(p);

  outputPinMenu();
}

void selectItemFromOutputPinMenu()
{
  int value = currentMenuHoverPosition-1;

  Serial.print("Value: ");
  Serial.println(value);

  pinMode(currentMenuPinSelection, OUTPUT);
  digitalWrite(currentMenuPinSelection, value);

  //outputPinMenu();
}

void inputMenu()
{
  currentMenuLevel = 2;
  currentMenuPosition = MENU_INPUT;
  currentMenuItemCount = TOTAL_ANALOG_PINS;

  resetMenu();
  display.clear();
}

void outputMenu()
{
  currentMenuLevel = 2;
  currentMenuPosition = MENU_OUTPUT;
  currentMenuItemCount = TOTAL_DIGITAL_PINS;

  resetMenu();
  display.clear();
}

void outputPinMenu()
{
  currentMenuLevel = 3;
  currentMenuPosition = MENU_OUTPUT_PIN;
  currentMenuItemCount = 2;

  resetMenu();
  display.clear();
}

void up()
{
  if (currentMenuHoverPosition == 1)
  {
    currentMenuHoverPosition = currentMenuItemCount;
    currentMenuScrollOffset = currentMenuItemCount - MENU_ROWS;
  }
  else
  {
    currentMenuHoverPosition--;
    if (currentMenuScrollOffset >= currentMenuHoverPosition)
      currentMenuScrollOffset--;
  }

  printMenuState();
}

void down()
{
  if (currentMenuHoverPosition >= currentMenuItemCount)
  {
    currentMenuHoverPosition = 1;
    currentMenuScrollOffset = 0;
  }
  else
  {
    currentMenuHoverPosition++;
    if (currentMenuHoverPosition > MENU_ROWS)
      currentMenuScrollOffset++;
  }

  printMenuState();
}

void back()
{
  Serial.println("Back");

  currentMenuPosition = currentMenuPosition / 10;

  display.clear();

  printMenuState();
}

void goHome()
{
  currentMenuLevel = 1;
  currentMenuPosition = 1;
  resetMenu();

  printMenuState();
}

void resetMenu()
{
  currentMenuHoverPosition = 1;
}

void printMenuState()
{
  Serial.print("Menu level: ");
  Serial.print(currentMenuLevel);
  Serial.print("; Menu hover position: ");
  Serial.print(currentMenuHoverPosition);
  Serial.print("; Scroll offset: ");
  Serial.println(currentMenuScrollOffset);
}

