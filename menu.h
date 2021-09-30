/*
 * menu.h
 *
 *  Created on: 10/12/2019
 *      Author: Carlos Enrique Apaza Chuctaya
 *				Nicolás Pardina Popp
 */

#ifndef MENU_H_
#define MENU_H_

#include <common_types.h>
#include <pbs.h>
#include <lcd.h>



// Mix, Record, Change, Exit
#define MEZCLAR_CTX	73
#define MENU_CTX 42
#define MENU_OPEN_CTX 144
#define MENU_VOLUMEN_CTX 55
#define MENU_BTCONF_CTX 156
#define GRABAR_MIC_CTX 88
#define MENU_MIX_CTX 199

#define NUM_OPTIONS 6
typedef enum {Mezclar, GrabarMic, ConfButtons, Volume, Efects} Options_t;

// Initialize the options
void menuInit();

// Waits for user to pick an option
void menu();

// Arrow points to next option in array
void nextOption();

// Executes user selection
void selectOption();

// Margins top, bottom, left, right
// Display all options, and an arrow pointing to current selection
// Using lcd puts(), new fn putArrow (uses  lcd_draw_hline, lcd_draw_hline, implement lcd_draw_diagonal?)
void displayMenu();

// When right button is pressed, interrupt to chose option


#endif /* MENU_H_ */
