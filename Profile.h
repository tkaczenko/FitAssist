#ifndef PROFILE_H
#define PROFILE_H

#include <QCoreApplication>
#include <QString>
#include <QDataStream>

/*!
 * \brief Serialized class for user's profile and calculating personal norms.
 *
 * This class calculates the all personal norms.
 * A list of norms:
 *  - BMI;
 *  - BMR;
 *  - Optimal weight;
 *  - Daily water norms;
 *
 * \author tkaczenko (Andrii Tkachenko)
 */
class Profile
{
    Q_DECLARE_TR_FUNCTIONS(Profile)

public:
    /// Create an empty Profile
    Profile();

    /*!
     * \brief Create profile with basic information
     * \param[in] w         User's weight
     * \param[in] h         User's height
     * \param[in] age       User's age
     * \param[in] g         Gender type
     * \param[in] a         User's activity coefficient
     * \param[in] calorie   Targeted number of daily calories
     */
    Profile(float w, short h, short age, QString g, float a, int calorie);

    ~Profile();

    // ostream, << overloading
    friend QDataStream &operator<<(QDataStream &out, const Profile &p)
    {
        out << p.getWeigth() << p.getHeigth() << p.getAge() << p.getGender()
            << p.getActivity() << p.getCalorie() << p.getBMI()
            << p.getConclusion() << p.getOptimalWeightMin() << p.getOptimalWeightMax()
            << p.getBMR() << p.getWater();
        return out;
    }

    // istream, >> overloading
    friend QDataStream &operator>>(QDataStream &in, Profile &p)
    {
        float weight;
        short height;
        short age;
        QString gender;
        float activity;
        int calorie;

        double bmi;
        QString conclusion;
        double minWeight;
        double maxWeight;
        double bmr;
        double water;

        in >> weight >> height >> age >> gender >> activity >> calorie >> bmi
                >> conclusion >> minWeight >> maxWeight >> bmr >> water;

        p = Profile(weight,height,age,gender,activity, calorie);
        p.setBMI(bmi);
        p.setConclusion(conclusion);
        p.setOptimalWeightMin(minWeight);
        p.setOptimalWeightMax(maxWeight);
        p.setBMR(bmr);
        p.setWater(water);

        return in;
    }

    /// Calculate the all profile norms
    void calculateNorms();

    void setWeigth(float w);
    float getWeigth() const;

    void setHeigth(short w);
    short getHeigth() const;

    void setAge(short age);
    short getAge() const;

    void setGender(QString g);
    QString getGender() const;

    void setActivity(float a);
    float getActivity() const;

    void setCalorie(int calorie);
    int getCalorie() const;

    void setBMI(double temp);
    double getBMI() const;

    void setConclusion(QString temp);
    QString getConclusion() const;

    void setOptimalWeightMin(double min);
    double getOptimalWeightMin() const;

    void setOptimalWeightMax(double max);
    double getOptimalWeightMax() const;

    void setBMR(double bmr);
    double getBMR() const;

    void setWater(double water);
    double getWater() const;

private:
    /*!
     * \brief Calculate BMI
     * \see https://en.wikipedia.org/wiki/Body_mass_index
     * \return BMI number
     */
    double calculateBMI();

    /*!
     * \brief Get conclusion by BMI number
     * \param[in] res   is the BMI number
     * \return Conlcusion
     */
    QString conclusionBMI(double res);

    /*!
     * \brief Calculate optimal weight by BMI
     * \param[in,out] min   killograms of minimum optimal weight
     * \param[in,out] max   killograms of maximum optimal weight
     */
    void calculateOptimalWeight(double &min, double &max);

    /*!
     * \brief Calculate BMR
     * \see https://en.wikipedia.org/wiki/Basal_metabolic_rate
     * \return BMR number
     */
    double calculateBMR();

    /*!
     * \brief Calculate optimal daily water norm.
     * \return Liters of optimal daily number of water
     */
    double calculateWater();

private:
    float weight;       /*!< User's weight(kg) */
    short height;       /*!< User's height(cm) */
    short age;          /*!< User's age */
    QString gender;     /*!< User's gender */
    float activity;     /*!< Coefficient of user's activity */
    int calorie;        /*!< Targeted number of daily calories */

    double bmi;
    QString conclusion; /*!< Conclusion by BMI */
    double minWeight;   /*!< Minimum of optimal weight */
    double maxWeight;   /*!< Maximum of optimal weigth */
    double bmr;         /*!< BMR number */
    double water;       /*!< Liters of optimal daily number of water */
};

#endif // PROFILE_H
