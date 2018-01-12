#define N 16

/*
void swap (int i, int j,int t[]) {
	int temp;
	temp = t[i];
	t[i] = t[j];
	t[j] = temp;
}

int partition (int t[], int g, int d) {
	int g0=g++;
	
	while (g<=d) {
    	if (t[g] <= t[g0]) {
			g++;
		} else {
			swap(g,d,t);
			d--;
		}
	}
	swap(g-1,g0,t);

	return g-1;
}

void tri_segmentation(int t[], int g, int d) {
	int p;
	
	if (g<d) {
		p = partition(t,g,d);
		tri_segmentation(t,g,p-1);
		tri_segmentation(t,p+1,d);
	}
}

int main(void)
{
	int t[N] = {5,16,8,8192,54,8,4096,9,-6,65,2,-1337,1024,2048,1,7};
	tri_segmentation(t,0,N-1);
	return 0;
}
*/

int main(void) {
	int t[N] = {5,16,8,8192,54,8,4096,9,-6,65,2,-1337,1024,2048,1,7};
	int i,j,p;

	for(i=1; i<N; i++) {
		p = t[i];
		j = i-1;
		while((j >= 0) && (t[j] > p)) {
			t[j+1] = t[j];
			j--;
		}
		t[j+1] = p;
	}
	
	return 0;
}

