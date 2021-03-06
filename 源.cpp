#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// find_position is uesd to find the exact position of a given character (eg. 'E' or 'e' in the scientific notation)
int find_position(char* stream, int size, const char target);

// compute_pow is used to compute the exponent part in the scientific notation
double compute_pow(int base, int exponent);

// convert_from_sci_to_float is used to compute the float from the string form, such like  '1.23e-3'
//first it identifies the decimal point, then uses convert_from_str_to_float to get the final value  
double convert_from_sci_to_float(char* stream, int size);

// convert_from_str_to_float is used to compute the float from the string form, such like '1.224' 
double convert_from_str_to_float(char* stream, int size);

// compute_expression is used to compute the value of the expression which is combined by numbers in float form 
double compute_expression(char* stream, int size);

int get_numberofdigit(char* stream, int size);

void print_result(double result);

int final_bit = 3;
/*
digit is used to identify a integer
signed_digit is used to identify negative integer and integer with positive sign
float is used to identify numbers with a decimal point
sci_notation is used to identify the numbers which in scientific notation form(eg.xxxx.xxxx e (-)yyy)
float_expression is used to identify the expression needed to be computed
identifier is used to identify the identifier that may appeas in an expression(eg.wdf_3 in the expression wdf_3+5+3e-5)
*/

int  find_position(char* stream, int size, const char target) {
	int i = 0;
	for (i = 0; i<size; i++) {
		if (stream[i] == target)
			break;
	}
	return i;
}

double compute_pow(int base, int exponent) {
	double result = 1;
	int i = 0;

	if (exponent > 0) {
		for (i = 0; i < exponent; i++)
			result = result * base;
	}

	if (exponent < 0) {
		for (i = 0; i > exponent; i--)
			result = result / base;
	}

	return result;
}

double convert_from_sci_to_float(char* stream, int size) {
	//in '1.23e5', pre_exp_num = '1.23', exp_num = '5'
	//exp_num_result = e5 = 10000, result = 12300
	double pre_exp_num = 0, exp_num = 0, exp_num_result = 0, result = 0;
	int pos_E = 0, pos_e = 0, pos_exp = 0;

	pos_E = find_position(stream, size, 'E');
	pos_e = find_position(stream, size, 'e');

	pos_exp = (pos_e >= pos_E) ? pos_E : pos_e;

	pre_exp_num = convert_from_str_to_float(stream, pos_exp);

	if (pos_exp < size)//means this number has exponent part which doesn't equal zero
		exp_num = convert_from_str_to_float(stream + pos_exp + 1, size - pos_exp - 1);
	else
		exp_num = 0;

	exp_num_result = compute_pow(10, exp_num);
	result = pre_exp_num * exp_num_result;

	return result;
}

double convert_from_str_to_float(char* stream, int size) {
	double result = 0, result_dot = 0;
	//lack bool class, so we use int flag to indicate the number is positive or negative
	int flag = 1;
	int pos_dot = 0, i = 0;

	if (stream[0] == '-') {
		flag = 0;
		stream++;
		size--;
	}
	else {
		if (stream[0] == '+') {
			stream++;
			size--;
		}
	}

	pos_dot = find_position(stream, size, '.');

	for (i = 0; i < pos_dot; i++) {
		if (stream[i] >= '0' && stream[i] <= '9')
			result = result * 10 + (stream[i] - '0');
		else continue;
	}

	for (i = size - 1; i > pos_dot; i--) {
		if (stream[i] >= '0' && stream[i] <= '9')
			result_dot = result_dot / 10 + (stream[i] - '0');
		else continue;
	}
	result = result + result_dot / 10;

	result = (flag == 1) ? result : (-result);

	return result;
}

double compute_expression(char* stream, int size) {
	double mid_result = 0;
	int pos_sym_sum = 0, pos_sym_sub = 0, mid_stop = 0, cur_bit = 0;

	if (size == 0)
		return 0;

	//there is no need to consider the first character of the strings  
	pos_sym_sum = find_position(stream + 1, size - 1, '+') + 1;
	//The float has '+' after 'e'or'E' shouldn't be identified as expression
	while (stream[pos_sym_sum - 1] == 'e' || stream[pos_sym_sum - 1] == 'E')
		pos_sym_sum += find_position(stream + pos_sym_sum + 1, size - 1 - pos_sym_sum, '+') + 1;

	pos_sym_sub = find_position(stream + 1, size - 1, '-') + 1;
	//The float has '-' after 'e'or'E' shouldn't be identified as expression
	while (stream[pos_sym_sub - 1] == 'e' || stream[pos_sym_sub - 1] == 'E')
		pos_sym_sub += find_position(stream + pos_sym_sub + 1, size - 1 - pos_sym_sub, '-') + 1;

	mid_stop = (pos_sym_sum > pos_sym_sub) ? pos_sym_sub : pos_sym_sum;

	cur_bit = get_numberofdigit(stream, mid_stop);
	final_bit = (cur_bit > final_bit) ? cur_bit : final_bit;

	mid_result = convert_from_sci_to_float(stream, mid_stop);

	return mid_result += compute_expression(stream + mid_stop, size - mid_stop);

}

int get_numberofdigit(char* stream, int size) {
	int bit = 0, pos_e = 0, pos_E = 0, pos_point = 0, cur_pos = 0;
	int digit = 0, digit_point = 0, digit_new = 0;

	pos_e = find_position(stream, size, 'e');
	pos_E = find_position(stream, size, 'E');
	pos_point = find_position(stream, size, '.');

	cur_pos = (pos_e < pos_E) ? pos_e : pos_E;

	if (cur_pos == size && pos_point == size)
		bit = 0;
	else {
		if (cur_pos < pos_point) {
			digit = convert_from_str_to_float(stream + cur_pos + 1, size - cur_pos - 1);
			bit = (digit > 0) ? 1 : -digit;
		}
		else {
			if (cur_pos == size) {
				if (stream[size - 1] >= '0' && stream[size - 1] <= '9')
					bit = size - pos_point - 1;
				else
					bit = size - pos_point - 2;
			}
			else {
				digit_point = convert_from_str_to_float(stream + cur_pos + 1, size - cur_pos - 1);
				if (digit_point > 0) {
					digit_new = cur_pos - pos_point - 1 - digit_point;
					if (digit_new > 0)
						bit = 0;
					else
						bit = -digit_new;
				}
				else
					bit = cur_pos - pos_point - 1 - digit_point;
			}
		}
	}

	return bit;
}

void print_result(double result) {
	int i = 0, n = 0, number = 0;
	long long int high = 0;
	int pro_x = 0, pro_y = 0, pro_z = 0;
	double low = 0, new_low = 0;
	char ch[20] = { 'a' };
	char re[20] = { 'a' };
	char *out = re;

	high = (long long int)result;
	while (high > 0) {
		ch[n] = '0' + high % 10;
		high = high / 10;
		n++;
	}
	high = (long long int)result;
	for (i = n; i > 0; i--) {
		*out = ch[i - 1];
		//printf("%s", out);
		*out++;
		number++;
	}
	if (n == 0) {
		*out = '0';
		*out++;
		//printf("%d", 0);
		number++;
	}
	if (final_bit > 0) {
		*out = '.';
		//printf("%s", out);
		number++;
		*out++;

		low = result - high;
		double preci = compute_pow(10, -final_bit);
		for (i = final_bit; i > 0; i--) {
			pro_x = low * 10;
			*out = '0' + pro_x;
			number++;
			//printf("%s", out);
			*out++;

			new_low = low * 10.0 - pro_x;
			pro_y = (new_low + preci) * 10;
			pro_z = new_low * 10;
			if (pro_y > pro_z)
				low = new_low + preci;
			else
				low = new_low;
		}
	}
	i = number;
	while (re[i - 1] == '0' ) {
		re[i - 1] = '\0';
		i--;
		if (re[i - 1] != '0')
			break;
	}
	printf("%s", re);
}

int main() {
	//char ch[12] = "0.045+0.055";
	//double result = compute_expression(ch, 12);
	print_result(0.100);
	return 0;
}