int stringLen(char* str){
	int len = 0;
	while (*str != '\0') {
		str++;
		len++;
	}
	//printf("len: %d",len);
	return len;
};
