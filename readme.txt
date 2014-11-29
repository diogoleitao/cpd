Divisão da matriz:
Seja p = # processos

p par: dividir N em 2;
       dividir M em (P/2);

p impar: Enviar N completo
		dividir M em p partes;

Numeracao das matrizes:
     |       |       |
  1  |   2   |   3   |
_____|_______|_______|
     |       |       |
  4  |   5   |   6   |    exemplo para p = 6
_____|_______|_______|



ReceiveInput:
	case 1: P is pair:
		1.1: top matrix
			1.1.1: not last block
			1.1.2: is last block
		1.2: Bottom matrix
			1.2.1: not last block
			1.2.2: is last block
	Case 2: p is odd:
		2.1: not last block
		2.2: is last block.