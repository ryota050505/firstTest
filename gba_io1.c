#include "gba1.h"
#include "8x8ascii.h"


/* �֐��v���g�^�C�v�錾 */
void locate(hword, hword);
void printh(hword);
void printn(hword);
void prints(byte*);
void print_ascii(byte);
void draw_point(hword, hword, hword);
hword mod(hword dividened, hword divisor);
hword div(hword dividened, hword divisor);


/* �O���[�o���ϐ����` */
point p;				/* ���W�w���p�\���� */


/* �������W(0, 0)�`(29,19)�̈ʒu(cx, cy)�����ʍ��W�ɕϊ� */
void locate(hword cx, hword cy){
	if (cx < LCD_CHAR_WIDTH ) {
		p.x = cx << 3;				/* x���W�̎w�� */
	} else {
		p.x = LCD_CHAR_WIDTH - 1;	/* x�̍ő��l���ݒ� */
	}
	if (cy < LCD_CHAR_HEIGHT) {
		p.y = cy << 3;				/* y���W�̎w�� */
	} else {
		p.y = LCD_CHAR_HEIGHT - 1;	/* y�̍ő��l���ݒ� */
	}
}


/* ������16�i���Ƃ��ĕ\�������֐��i�ő��l16�r�b�g�j*/
void printh(hword val) {

	byte char_data[] = "0123456789ABCDEF";
	byte buf[7];
	hword tmp;
	int i;

	/* ���͒lval�̌������� */
	tmp = val;
	for (i = 0; i < 4; i++) {
		if (tmp > 0xF){
			tmp = tmp >> 4;
		} else {
			break;
		}
	}

	/* �������̍Ō���NULL�R�[�h�}���i"0x"�̐擪2�������̒l��i�ɉ��Z�j*/
	buf[i+3] = 0;

	/* �ŉ��ʌ��̕������}�����C���͒l��4�r�b�g�V�t�g�i"0x"�̐擪2�������̒l��i�ɉ��Z�j*/
	for (; i >= 0; i--) {
		buf[i+2] = char_data[val & 0xF];
		val = val >> 4;
	}

	/* �������擪��16�i����"0x"���}�� */
	buf[1] = 'x';
	buf[0] = '0';

	/* �������S�̂��\�� */
	prints(buf);

	return;
}


/* ������10�i���Ƃ��ĕ\�������֐��i�ő��l16�r�b�g�j*/
void printn(hword val) {

	byte char_data[] = "0123456789";
	byte buf[6];
	hword tmp;
	int i;

	/* ���͒lval�̌������� */
	tmp = val;
	for (i = 0; i < 5; i++) {
		if (tmp > 10){
			tmp = div(tmp, 10);
		} else {
			break;
		}
	}

	/* �������̍Ō���NULL�R�[�h�}�� */
	buf[i+1] = 0;

	/* �ŉ��ʌ��̕����i10�Ŋ������]���j���}�����C���͒l��10�Ŋ��� */
	for(; i >= 0; i--) {
		buf[i] = char_data[mod(val, 10)];
		val = div(val, 10);
	}

	/* �������S�̂��\�� */
	prints(buf);

	return;
}


/* �����Z�̏������߂��֐�*/
hword div(hword dividened, hword divisor){

	hword quotient = 0;			/* �� */

	while(1){
		if (dividened >= divisor){
			dividened = dividened - divisor;
			quotient++;
		} else {
			break;
		}
	}
	return quotient;
}


/* �����Z�̗]�������߂��֐�*/
hword mod(hword dividened, hword divisor){

	hword quotient = 0;			/* �� */

	while(1){
		if (dividened >= divisor){
			dividened = dividened - divisor;
			quotient++;
		} else {
			break;
		}
	}
	return dividened;
}


/* �������\���֐� */
void prints(byte *str) {

	while(*str) {
		print_ascii(*str++);
		p.x = p.x + CHAR_WIDTH;			/* �\���ʒu��1�����E�� */
	}
}


/* locate�Őݒ肳�ꂽ�ʒu��ASCII�R�[�hascii_num�ɑ������镶�����\�������֐� */
void print_ascii(byte ascii_num) {

	hword tx, ty;							/* �����p�^�[���̍��W�w�� */
	byte cbit;								/* �r�b�g���� */

	for(ty = 0; ty < 8; ty++) {				/* �����p�^�[����y���W�w�� */

		cbit = 0x80;						/* �����r�b�g��8�r�b�g�̍ŏ��ʂɃZ�b�g */

		for(tx = 0; tx < 8; tx++) {							/* �����p�^�[����x���W�w�� */
			if((char8x8[ascii_num][ty] & cbit) == cbit){			/* �r�b�g�ʒu�̔��� */
				draw_point((p.x + tx), (p.y + ty), BGR(0x1F, 0x1F, 0x1F));	// 1�Ȃ甒
			}else {
				draw_point((p.x + tx), (p.y + ty), BGR(0x00, 0x00, 0x00));	// 0�Ȃ獕
			}
			cbit = cbit >> 1;					/* �����r�b�g��1�r�b�g�E�փV�t�g */
		}
	}
}


/* �w�肵�����W(x, y) �Ɏw�肵���Fcolor�̓_���`�悷���֐� */
void draw_point(hword x, hword y, hword color) {

	hword *ptr;
	ptr = (hword *) VRAM;

	ptr = ptr + x + y * LCD_WIDTH;
	*ptr = color;

}
