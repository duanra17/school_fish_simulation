
struct poisson{
    double x; //Position abscisses
    double y; //Position ordonnées
    double dir; //Direction de la vitesse
};

void initialisation(struct poisson P, double x_max, double y_max);

float distance(struct poisson A, struct poisson B);

int dans_angle_mort(struct poisson A, struct poisson B);

void traitement(int* indices_za, int* indices_zr, int* indices_zo, int indP, double* dir_temp, int N)