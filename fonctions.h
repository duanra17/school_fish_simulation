
struct poisson{
    double x; //Position abscisses
    double y; //Position ordonnées
    double dir; //Direction de la vitesse
};

void initialisation(struct poisson P, double x_max, double y_max);

float distance(struct poisson A, struct poisson B);

int dans_angle_mort(struct poisson A, struct poisson B);

double repulsion(int* indices_zr, int N, struct poisson* banc);

double attraction(int* indices_za, int N, struct poisson* banc);

double orientation(int* indices_zo, int N, struct poisson* banc);

void traitement(int* indices_za, int* indices_zr, int* indices_zo, double* dir_temp, int N, struct poisson* banc, int indP, float s);

float gaussienne(double mu, double sigma);