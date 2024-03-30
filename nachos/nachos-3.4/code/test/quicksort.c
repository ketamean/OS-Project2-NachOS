#include "syscall.h"

void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int partition(int arr[], int left, int right) {
	int pivot = arr[right];
	int j = left;
	int i = (left - 1);
	for(j; j <= right - 1; j++) {
		if(arr[j] < pivot) {
			i++;
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[right]);
	return (i + 1);
}

void quicksort(int arr[], int left, int right) {
	if(left < right) {
		int parti = partition(arr, left, right);
		quicksort(arr, left, parti - 1);
		quicksort(arr, parti + 1, right);
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
	// Doc cac phan tu
	for (i = 0; i < n; i++) {
		PrintString("Enter an integer: ");
		arr[i] = ReadInt();
	}
	// Chay thuat toan quicksort
	quicksort(arr, 0, n - 1);
	// Tao file va mo file de ghi du lieu
	CreateF("quicksort.txt");
	file = OpenF("quicksort.txt", 0);
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
	Halt();
}