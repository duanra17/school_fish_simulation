
struct poisson{
    double x; //Position abscisses
    double y; //Position ordonnées
    double dir; //Direction de la vitesse
};

void initialisation(struct poisson* P, double x_max, double y_max);

double distance(struct poisson A, struct poisson B);

double modulo360(double A);

int dans_angle_mort(struct poisson A, struct poisson B, double alpha);

double repulsion(int* indices_zr, int N, struct poisson* banc);

double attraction(int* indices_za, int N, struct poisson* banc);

double orientation(int* indices_zo, int N, struct poisson* banc);

int traitement(int* indices_za, int* indices_zr, int* indices_zo, double* dir_temp, int N, struct poisson* banc, int indP);

double gaussienne(double mu, double sigma);