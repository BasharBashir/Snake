#include <conf.h>
#include <kernel.h>
#include <dos.h>

extern int row, col;
extern int Esc;
extern int flag;
extern int right_press, left_press;
extern up, down;
int location;
int counter;
int Attribute[25][40];
extern int tod, timer, latch;
void DrowLeft();
void DrowRight();
void RemoveLeft();
void RemoveRight();
void DrowScreen();
void InitAttribute();
void AttributeChecker();
xmain(){

    int Inc();
    InitAttribute();
    resume(create(Inc, INITSTK, INITPRIO + 1, "proc 4", 0));
    printf("end game\n");
    return 0;
}

Inc(int ptr){
    int right = 1, left = 0,i;
    latch = 65536;
    DrowScreen();
    DrowRight();
    while (!Esc) {
        AttributeChecker();
        if (timer == 1 && flag) {
            if (right && col+8<40) {
                RemoveRight();
                col++;
                for (i = 0; i < 6; i++)
                    Attribute[row][col + i]++;
                DrowRight();
            }
            if (left && col - 7 > 0) {
                RemoveLeft();
                col--;
                for (i = 0; i < 6; i++)
                    Attribute[row][col - i]++;
                DrowLeft();
            }
            timer = 0;
        }
        if (left_press && right) {
            RemoveRight();
            col = col + 7;
            for (i = 0; i < 6; i++)
                Attribute[row][col - i]++;
            DrowLeft();
            left_press = 0;
            left = 1;
            right = 0;
        }
        if (right_press && left) {
            RemoveLeft();
            col = col - 7;
            for (i = 0; i < 6; i++)
                Attribute[row][col + i]++;
            DrowRight();
            right_press = 0;
            right = 1;
            left = 0;
        }
        if (up) {
            if (row > 0) {
                if (right) {
                    RemoveRight();
                    row--;
                    for (i = 0; i < 6; i++)
                        Attribute[row][col + i]++;
                    DrowRight();
                }
                if (left) {
                    RemoveLeft();
                    row--;
                    for (i = 0; i < 6; i++)
                        Attribute[row][col - i]++;
                    DrowLeft();
                }
            }
            up = 0;
        }
        if (down) {
            if (row < 24) {
                if (right) {
                    RemoveRight();
                    row++;
                    for (i = 0; i < 6; i++)
                        Attribute[row][col + i]++;
                    DrowRight();
                }
                if (left) {
                    RemoveLeft();
                    row++;
                    for (i = 0; i < 6; i++)
                        Attribute[row][col - i]++;
                    DrowLeft();
                }
            }
            down = 0;
        }
    }
    asm{
        MOV  AX,2
        INT  10h
    }
    kill(getpid());
    return 0; // removing warning
}

void DrowRight() {
    int i,attribute,index,Cur;
    location = 2 * (row * 40 + col);
    for (i = 0; i < 8; i++) {
        attribute = Attribute[row][col + i];
        index = location + 2 * i;
        asm{
            MOV DI,[location]
            MOV AX,0B800h                        
            MOV ES,AX     
            MOV DI,[index]      
            MOV DX, [attribute]
            MOV AH, DL  
            MOV ES : [DI] , AX
            ADD DI, 2
        }
    }
    asm{   
        MOV DI,[location]
        MOV BYTE PTR ES : [DI + 0] , '-'
        MOV BYTE PTR ES : [DI + 2] , '-'
        MOV BYTE PTR ES : [DI + 4] , '-'
        MOV BYTE PTR ES : [DI + 6] , '-'
        MOV BYTE PTR ES : [DI + 8] , '-'
        MOV BYTE PTR ES : [DI + 10] , '>'
        MOV BYTE PTR ES : [DI + 12] , '['
        MOV BYTE PTR ES : [DI + 14] , ']'
    }
    Cur = (location + 12) / 2;
    asm{
        MOV BX,[Cur]
        MOV              DX,3D4H  // Point to 3D4H - 3D5H port pair
        MOV              AL,14   // Address of cursor register pos high byte
        MOV              AH,BH    // Get desired value of cursor pos high byte
        OUT              DX,AX    // Port(3D4h) = 14, Port(3D5h) = Value of BH
        MOV              AL,15    // Address of cursor register pos low byte
        MOV              AH,BL    // Get desired value pf cursor pos low byte
        OUT              DX,AX    // Port(3D4h) = 15, Port(3D5h) = Value of BL

    }
}

void RemoveRight() {
    location = 2 * (row * 40 + col);
    asm{
       MOV AL,' '
       MOV AH,00011000B
       MOV DI,[location]
       MOV ES : [DI] , AX
       MOV ES : [DI + 2] , AX
       MOV ES : [DI + 4] , AX
       MOV ES : [DI + 6] , AX
       MOV ES : [DI + 8] , AX
       MOV ES : [DI + 10] , AX
       MOV ES : [DI + 12] , AX
       MOV ES : [DI + 14] , AX
    }
    
}

void RemoveLeft() {
    location = 2 * (row * 40 + col);
    asm{
      MOV AL,' '        
      MOV AH,00011000B  
      MOV DI,[location]
      MOV ES : [DI] , AX
      MOV ES : [DI - 2] , AX
      MOV ES : [DI - 4] , AX
      MOV ES : [DI - 6] , AX
      MOV ES : [DI - 8] , AX
      MOV ES : [DI - 10] , AX
      MOV ES : [DI - 12] , AX
      MOV ES : [DI - 14] , AX
    }
}

void DrowLeft() {
    int i, attribute, index;
    int Cur;
    location = 2 * (row * 40 + col);
    for (i = 0; i < 8; i++) {
        attribute = Attribute[row][col - i];
        index = location - 2 * i;
        asm{
            MOV DI,[location]
            MOV AX,0B800h
            MOV ES,AX
            MOV DI,[index]
            MOV DX,[attribute]
            MOV AH, DL
            MOV ES : [DI] , AX
            SUB DI, 2
        }
    }
    asm{
        MOV DI,[location]
        MOV BYTE PTR ES : [DI] , '-'
        MOV BYTE PTR ES : [DI - 2] , '-'
        MOV BYTE PTR ES : [DI - 4] , '-'
        MOV BYTE PTR ES : [DI - 6] , '-'
        MOV BYTE PTR ES : [DI - 8] , '-'
        MOV BYTE PTR ES : [DI - 10] , '<'
        MOV BYTE PTR ES : [DI - 12] , ']'
        MOV BYTE PTR ES : [DI - 14] , '['
    }
    Cur = (location - 12) / 2;
    asm{
        MOV BX,[Cur]
        MOV              DX,3D4H  // Point to 3D4H - 3D5H port pair
        MOV              AL,14   // Address of cursor register pos high byte
        MOV              AH,BH    // Get desired value of cursor pos high byte
        OUT              DX,AX    // Port(3D4h) = 14, Port(3D5h) = Value of BH
        MOV              AL,15    // Address of cursor register pos low byte
        MOV              AH,BL    // Get desired value pf cursor pos low byte
        OUT              DX,AX    // Port(3D4h) = 15, Port(3D5h) = Value of BL
    }
}
void DrowScreen() {
    asm{
        MOV AH,0          // Select function = 'Set mode'
        MOV AL,1          // 40 by 25 color image
        INT 10h           // Adapter initialized.Page 0 displayed
        MOV AX,0B800h     // Segment address of memory on color adapter
        MOV ES,AX         // Set up extra segment register
        MOV DI,0          // Initial offset address into segment
        MOV AL,' '        // Character space to fill adapter memory
        MOV AH,00011000B  // Attribute byte : Intense yellow
        MOV CX,1000       // Initialize count, 1 Screen
        CLD               // Write forward
        REP STOSW
    }
}
void InitAttribute() {
    int i, j;
    for (i = 0; i < 25; i++)
        for (j = 0; j < 40; j++)
            Attribute[i][j] = 32;
}
void AttributeChecker() {
    int i, j;
    for (i = 0; i < 25; i++)
        for (j = 0; j < 40; j++)
            if (Attribute[i][j] > 255)
                Attribute[i][j] = 0;
}