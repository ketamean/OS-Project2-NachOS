#include "syscall.h"

void merge(int arr[], int left, int mid, int right) {
    	int i, j, k;
	// So luong phan tu con ben trai
    	int n_left = mid - left + 1;
	// So luong phan tu con ben phai
    	int n_right = right - mid; 
 
    	int Left_arr[n_left], Right_arr[n_right];
 	// Sao chep phan tu vao mang tuong ung
    	for (i = 0; i < n_left; i++)
        	Left_arr[i] = arr[left + i];
    	for (j = 0; j < n_right; j++)
        	Right_arr[j] = arr[mid + 1 + j];
 
    	i = 0;
    	j = 0;
    	k = left;
	// Gop cac phan tu vao mang arr
    	while (i < n_left && j < n_right) {
        	if (Left_arr[i] <= Right_arr[j]) {
            		arr[k] = Left_arr[i];
            		i++;
        	}
        	else {
            		arr[k] = Right_arr[j];
            		j++;
        	}
        	k++;
    	}
 
    	while (i < n_left) {
        	arr[k] = Left_arr[i];
        	i++;
        	k++;
    	}
 
    	while (j < n_right) {
        	arr[k] = Right_arr[j];
        	j++;
        	k++;
    	}
}

void mergesort(int arr[], int left, int right) {
	int mid;
    	if (left < right) {
        	mid = left + (right - left) / 2;
		// De quy
        	mergesort(arr, left, mid);
        	mergesort(arr, mid + 1, right);
        	merge(arr, left, mid, right);
    	}
}

int main() {
	OpenFileId file;
	int n;
	int i;
	int j;
	int arr[100];
	int num, divisor;
	PrintString("Input an number of array, n < 100: ");
	// Doc vao so luong phan tu
	n = ReadInt();
	if (n < 100) {
		// Doc cac phan tu
		for (i = 0; i < n; i++) {
			PrintString("Enter an integer: ");
			arr[i] = ReadInt();
		}
		// Chay thuat toan mergesort
		mergesort(arr, 0, n - 1);
		// Tao file va mo file de ghi du lieu
		CreateF("mergesort.txt");
		file = OpenF("mergesort.txt", 0);
		PrintString("Done sorting");
	   	for (i = 0; i < n; i++) {
			// Gioi han mot so chi co 10 chi so
			char buffer[11]; 
			num = arr[i];
			j = 0;
			// Neu la so am, chuyen thanh so duong
			if (num < 0) {
		    		buffer[j++] = '-';
		    		num = -num;
			}
			// Xu ly so de ghi vao file
			divisor = 1;
			while (num / divisor > 9) {
		    		divisor *= 10;
			}
			while (divisor != 0) {
		    		buffer[j++] = num / divisor + '0';
		    		num %= divisor;
		    		divisor /= 10;
			}
			// Them ky tu ket thuc chuoi
			buffer[j] = '\0';
			// Thuc hien ghi du lieu doc duoc vao file
			WriteF(buffer, j, file);
			// Ghi dau cach giua moi chu so
			WriteF(" ", 1, file);
	    	}
		CloseF(file);
	}
	else {
		PrintString("So luong vuot qua gioi han\n");
	}
	Halt();
}