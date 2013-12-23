#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct vec{
	float
		i, j, k
	;
} Vector, *VectorPtr;

typedef struct wrld{
/*
	World
		Vectors
			e	eye
			p	plane position
			r	array of rays

		Scalars
			pxw	pixel width (int)
			pxh	pixel height (int)
			plw	plane width (float)
			plh	plane height (float)
*/
	Vector v[4];
	VectorPtr e, p; /* v[0], v[1] */
	float /* v[2] */
		*pxw, *pxh; /* 0 */
	float /* v[3] */
		*plw, *plh; /* 0 */
	VectorPtr r; /* ray array */
} World, *WorldPtr;

float vdotp(VectorPtr v, VectorPtr w);
float vmag(VectorPtr v);
int makeworld(VectorPtr v, int l, WorldPtr w);
int plotpoints(VectorPtr v, int l, WorldPtr w);
int scanvecs(VectorPtr *v);
int vcpy(VectorPtr v, VectorPtr c);
int vcrossp(VectorPtr v, VectorPtr w, VectorPtr p);
int vdisp(VectorPtr v);
int vfill(float a, float b, float c, VectorPtr v);
int vnorm(VectorPtr v, VectorPtr w);
int vsmult(VectorPtr v, float s, VectorPtr m);
int vsub(VectorPtr v, VectorPtr w, VectorPtr d);
int vsum(VectorPtr v, VectorPtr w, VectorPtr s);
VectorPtr vcalloc();

int main(){
	VectorPtr v;
	World w;

	int l = scanvecs(&v);
	makeworld(v, l, &w);
	plotpoints(v, l, &w);

	return 0;
}

VectorPtr vcalloc(){
	return (VectorPtr) malloc(sizeof(Vector));
}

/* m = (s)(v) */
int vsmult(VectorPtr v, float s, VectorPtr m){
	(*m).i = s * (*v).i;
	(*m).j = s * (*v).j;
	(*m).k = s * (*v).k;
	return 0;
}

/* p = v x w */
int vcrossp(VectorPtr v, VectorPtr w, VectorPtr p){
	float
		i = (*v).j * (*w).k - (*v).k * (*w).j,
		j = (*v).k * (*w).i - (*v).i * (*w).k,
		k = (*v).i * (*w).j - (*v).j * (*w).i
	;
	vfill(i, j, k, p);
	return 0;
}

/* s = v + w */
int vsum(VectorPtr v, VectorPtr w, VectorPtr s){
	(*s).i = (*v).i + (*w).i;
	(*s).j = (*v).j + (*w).j;
	(*s).k = (*v).k + (*w).k;
	return 0;
}

/* d = v - w */
int vsub(VectorPtr v, VectorPtr w, VectorPtr d){
	(*d).i = (*v).i - (*w).i;
	(*d).j = (*v).j - (*w).j;
	(*d).k = (*v).k - (*w).k;
	return 0;
}

/* w = v / ||v|| */
int vnorm(VectorPtr v, VectorPtr w){
	Vector r;
	vsmult(v, 1 / vmag(v), &r);
	vcpy(&r, w);
	return 0;
}

/* v = ai + bj + ck */
int vfill(float a, float b, float c, VectorPtr v){
	(*v).i = a;
	(*v).j = b;
	(*v).k = c;
	return 0;
}

/* c = v */
int vcpy(VectorPtr v, VectorPtr c){
	vfill((*v).i, (*v).j, (*v).k, c);
	return 0;
}

int vdisp(VectorPtr v){
	printf(
		"%fi + %fj + %fk\n",
		(*v).i, (*v).j, (*v).k
	);
	return 0;
}

/* v * w */
float vdotp(VectorPtr v, VectorPtr w){
	return
		(*v).i * (*w).i +
		(*v).j * (*w).j +
		(*v).k * (*w).k
	;
}

/* ||v|| */
float vmag(VectorPtr v){
	return
		sqrt(
			(*v).i * (*v).i +
			(*v).j * (*v).j +
			(*v).k * (*v).k
		)
	;
}

int scanvecs(VectorPtr *v){
	(*v) = vcalloc();
	int i = 0;
	while(
		scanf(
			"%f %f %f",
			&((*v)[i].i),
			&((*v)[i].j),
			&((*v)[i].k)
		) != EOF
	)
		(*v) = (VectorPtr) realloc(
			(*v),
			sizeof(Vector) * ((++ i) + 1)
		);

	return i;
}

/* w = v-based world */
int makeworld(VectorPtr v, int l, WorldPtr w){
	int i, j, k;

	if(l < 4)
		return 1;

	(*w).e = &((*w).v[0]);
	(*w).p = &((*w).v[1]);
	(*w).pxw = &((*w).v[2].i);
	(*w).pxh = &((*w).v[2].j);
	(*w).plw = &((*w).v[3].i);
	(*w).plh = &((*w).v[3].j);

	for(i = 0; i < 4; i ++){
		((*w).v[i]).i = v[i].i;
		((*w).v[i]).j = v[i].j;
		((*w).v[i]).k = v[i].k;
	}

	float
		pxw = *(*w).pxw,
		pxh = *(*w).pxh,
		plw = *(*w).plw,
		plh = *(*w).plh,
		dw = plw / (pxw - 1),
		dh = plh / (pxh - 1)
	;

	Vector
		e, p, pe,
		pi, pj, pic, pjc, pij, ray,
		u
	;

	vcpy((*w).e, &e);
	vcpy((*w).p, &p);
	vfill(1, 0, 0, &u);

	vsub(&e, &p, &pe);

	vcrossp(&pe, &u, &pj);
	vcrossp(&pj, &pe, &pi);

	vnorm(&pj, &pj);
	vnorm(&pi, &pi);

	(*w).r = (VectorPtr) malloc(
		sizeof(Vector) * pxw * pxh
	);

	k = 0;
	for(i = 0; i < pxw; i ++)
		for(j = 0; j < pxh; j ++){
			vsmult(&pi, plw / 2 - dw * i, &pic);
			vsmult(&pj, plh / 2 - dw * j, &pjc);

			vsum(&pic, &pjc, &pij);
			vsum(&pij, &p, &pij);

			vsub(&pij, &e, &ray);
			vnorm(&ray, &ray);

			vcpy(&ray, (*w).r + (k ++));
		}

	return 0;
}

int plotpoints(VectorPtr v, int l, WorldPtr w){
	if(l < 5)
		return 1;

	float
		pxw = *(*w).pxw,
		pxh = *(*w).pxh,
		plw = *(*w).plw,
		plh = *(*w).plh,
		num, den, sproja, sprojb,
		pxi, pxj
	;

	Vector
		e, p, pe,
		pi, pj,
		a, b, n, p0,
		*l0, lv, bfr, pt,
		u
	;

	vcpy((*w).e, &e);
	vcpy((*w).p, &p);
/* u is invalid for certain values of p and e...
	...Fix This! */
	if(e.k < 0)
		vfill(-1, 0, 0, &u);
	else
		vfill(1, 0, 0, &u);


	vsub(&e, &p, &pe);

	vcrossp(&u, &pe, &pj);
	vcrossp(&pj, &pe, &pi);
	vcrossp(&pj, &pi, &n);

	vnorm(&pj, &pj);
	vnorm(&pi, &pi);
	vnorm(&n, &n);

	vsmult(&pi, plw / 2, &a);
	vsmult(&pj, plh / 2, &b);

	vsub(&p, &a, &p0);
	vsub(&p0, &b, &p0);

	vsmult(&pi, plw, &a);
	vsmult(&pj, plh, &b);

	VectorPtr *pxmx = (VectorPtr *) malloc(
		sizeof(VectorPtr) * pxh
	);

	int i;
	for(i = 0; i < pxh; i ++)
		pxmx[i] = (VectorPtr) malloc(
			sizeof(Vector) * pxw
		);

	int j;
	for(i = 0; i < pxh; i ++)
		for(j = 0; j < pxw; j ++){
			vfill(0, 0, 0, &pxmx[i][j]);
		}

	for(i = 4; i < l; i ++){
		l0 = &e;

		vsub(v + i, l0, &lv);
		vnorm(&lv, &lv);

		vsub(&p0, l0, &bfr);
		num = vdotp(&bfr, &n);
		den = vdotp(&lv, &n);

		vsmult(&lv, num / den, &pt);
		vsum(&pt, l0, &pt);
		vsub(&pt, &p0, &pt);

		sproja = vdotp(&pt, &a) / vmag(&a);
		sprojb = vdotp(&pt, &b) / vmag(&b);

		if(
			sproja >= 0 &&
			sproja <= vmag(&a) &&
			sprojb >= 0 &&
			sprojb <= vmag(&b)
		) {
			pxi = (sproja / vmag(&a)) * pxw;
			pxj = (sprojb / vmag(&b)) * pxh;
			vfill(
				255, 255, 255,
				&pxmx[(int) pxj][(int) pxi]
			);
		}
	}

	printf(
		"P3\n%d %d\n256\n",
		(int) pxw, (int) pxh
	);
	for(i = 0; i < pxw; i ++)
		for(j = 0; j < pxh; j ++){
			printf(
				"%3d %3d %3d\n",
				(int) pxmx[i][j].i,
				(int) pxmx[i][j].j,
				(int) pxmx[i][j].k
			);
		}

	return 0;
}
