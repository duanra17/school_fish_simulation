
struct poisson{
    double x; //Position abscisses
    double y; //Position ordonnées
    double dir; //Direction de la vitesse
};

void initialisation(struct poisson P, double x_max, double y_max);

float distance(struct poisson A, struct poisson B);

int dans_angle_mort(struct poisson A, struct poisson B);

