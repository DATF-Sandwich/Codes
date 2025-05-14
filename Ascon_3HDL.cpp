#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <cmath>
#include <map>
#include <time.h>
#include <string>
#define round 6
using namespace std;


const int N0 = 2;
const int VARIABLE_COUNT = 128 + 128 + 4 + 8 * 320 * round;       // key(128) + IV(128) + x,y,z + const 1 + ATF

const int loc_x = 256;
const int loc_y = 257;
const int loc_z = 258;
const int loc_1 = 259;
using PolyTerm = bitset<VARIABLE_COUNT>;
using Poly = vector<PolyTerm>;

//   Part 1: Output by Format
string polyToString(const Poly& poly)
{
    string result;
    for (const auto& term : poly)
    {
        result += term.to_string();
        result += " ";
    }
    return result;
}
struct PolyCompare
{
    bool operator()(const Poly& a, const Poly& b) const
    {
        return polyToString(a) < polyToString(b);
    }
};
bool MySort(PolyTerm a, PolyTerm b)
{
    int i;
    for (i = 0; i < loc_x; i++)
    {
        if (a[i] > b[i])
            return true;
        else if (a[i] < b[i])
            return false;
    }
    for (i = loc_1 + 1; i < VARIABLE_COUNT; i++)
    {
        if (a[i] > b[i])
            return true;
        else if (a[i] < b[i])
            return false;
    }
    if (a[loc_x] > b[loc_x])
        return true;
    else if (a[loc_x] < b[loc_x])
        return false;

    if (a[loc_y] > b[loc_y])
        return true;
    else if (a[loc_y] < b[loc_y])
        return false;

    if (a[loc_z] > b[loc_z])
        return true;
    else if (a[loc_z] < b[loc_z])
        return false;

    if (a[loc_1] > b[loc_1])
        return true;
    else if (a[loc_1] < b[loc_1])
        return false;

    return false;
}

void printPoly(Poly poly)
{
    int i;
    for (auto& term : poly)
    {
        for (i = 0; i < 128; i++)
            if (term[i] == 1)   cout << "k" << i;
        for (i = 0; i < 128; i++)
            if (term[i + 128] == 1) cout << "v" << i;
        for (i = 0; i < 320 * round; i++)
            if (term[i + loc_1 + 1] == 1)   cout << "a" << i;
        for (i = 0; i < 320 * round; i++)
            if (term[i + loc_1 + 1 + 320 * round] == 1) cout << "b" << i;
        for (i = 0; i < 320 * round; i++)
            if (term[i + loc_1 + 1 + 2 * 320 * round] == 1) cout << "c" << i;
        for (i = 0; i < 320 * round; i++)
            if (term[i + loc_1 + 1 + 3 * 320 * round] == 1) cout << "d" << i;
        for (i = 0; i < 320 * round; i++)
            if (term[i + loc_1 + 1 + 4 * 320 * round] == 1) cout << "e" << i;
        for (i = 0; i < 320 * round; i++)
            if (term[i + loc_1 + 1 + 5 * 320 * round] == 1) cout << "f" << i;
        for (i = 0; i < 320 * round; i++)
            if (term[i + loc_1 + 1 + 6 * 320 * round] == 1) cout << "g" << i;
        for (i = 0; i < 320 * round; i++)
            if (term[i + loc_1 + 1 + 7 * 320 * round] == 1) cout << "h" << i;

        if (term[loc_x] == 1)   cout << "x";
        if (term[loc_y] == 1)   cout << "y";
        if (term[loc_z] == 1)   cout << "z";
        if (term[loc_1] == 1)   cout << "1";
        cout << "+";
    }
    cout << endl;
}


//   Part 2: Calculating the DATF
Poly add(Poly a, Poly b)
{
    Poly result;
    unordered_map<PolyTerm, int> termCount;

    for (auto& term : a)
        termCount[term]++;
    for (auto& term : b)
        termCount[term]++;

    for (const auto& pair : termCount)
        if (pair.second % 2 == 1)
            result.push_back(pair.first);

    sort(result.begin(), result.end(), MySort);
    return result;
}
Poly add_one(Poly a)
{
    sort(a.begin(), a.end(), MySort);

    PolyTerm term;
    term.set(loc_1);
    if (a.size() == 0)
    {
        Poly one;
        one.push_back(term);
        return one;
    }
    else
    {
        if (a[a.size() - 1][loc_1] == 1)
            a.pop_back();
        else
            a.push_back(term);
        return a;
    }
}
Poly multiply(Poly a, Poly b)
{
    Poly result;
    unordered_map<PolyTerm, int> occurrences;
    for (auto& termA : a)
    {
        for (auto& termB : b)
        {
            PolyTerm newTerm = termA | termB;
            if (newTerm[loc_1] == 1 && newTerm.count() > 1)
                newTerm[loc_1] = 0;
            occurrences[newTerm]++;
        }
    }
    for (auto& pair : occurrences)
        if (pair.second % 2 == 1)
            result.push_back(pair.first);
    sort(result.begin(), result.end(), MySort);
    return result;
}
bool isInverse(const Poly& a, const Poly& b)
{
    PolyTerm constant1_term;
    constant1_term.set(loc_1);
    Poly plus = add(a, b);
    return plus[0] == constant1_term;
}
bool isContain(Poly& poly, Poly& subPoly)
{
    for (auto& term : subPoly)
        if (find(poly.begin(), poly.end(), term) == poly.end())
            return false;
    return true;
}
bool isReplace(Poly poly)
{
    sort(poly.begin(), poly.end(), MySort);
    PolyTerm variable;
    for (auto& term : poly)
        variable = variable | term;
    variable[loc_1] = 0;
    if (variable.count() <= 2)
        return false;
    if (poly.size() <= 1)
        return false;
    if (poly.size() == 2 && poly[1][loc_1] == 1)
        return false;
    return true;

}

int Judge_Poly(Poly poly)
{
    if (poly.size() == 0)
        return -3;
    else if (poly.size() == 1 && poly[0].count() == 1)
    {
        if (poly[0][loc_1] == 1)
            return -2;
        for (int i = 0; i < VARIABLE_COUNT; i++)
            if (poly[0][i] == 1)
                return i;
    }
    return -1;
}

vector<Poly> zeroizePolynomials(vector<Poly> s)
{
    vector<Poly> zeroized_polys;

    while (true)
    {
        map<Poly, int, PolyCompare> count;
        for (auto& poly : s)
        {
            sort(poly.begin(), poly.end(), MySort);
            if (poly[0][loc_1] == 0)
            {
                count[poly]++;
            }
        }

        Poly max_poly;
        int max_count = 0;
        for (auto& pair : count)
            if (pair.second > max_count)
            {
                max_count = pair.second;
                max_poly = pair.first;
            }
        //printPoly(max_poly);
        zeroized_polys.push_back(max_poly);

        vector<Poly> new_s;
        for (auto& poly : s)
        {
            if (isContain(poly, max_poly))
                poly = add(poly, max_poly);

            if (poly.size() > 1)
                new_s.push_back(poly);
            else if (poly.size() == 1 && poly[0][loc_1] == 0)
                new_s.push_back(poly);
        }
        if (new_s.size() == 0)
            break;
        s = new_s;
    }
    return zeroized_polys;
}
int zeroize(Poly poly, vector<Poly>& zero)
{
    for (auto& t : zero)
    {
        if (isContain(poly, t))
        {
            poly = add(poly, t);
            if (poly.size() == 0)
                return -3;
            else if (poly.size() == 1 && poly[0][loc_1] == 1)
                return -2;
        }
    }
}

void DATF(Poly poly, Poly& a, Poly& b, Poly& c, Poly& d, Poly& e, Poly& f, Poly& g, Poly& h)
{
    for (auto term : poly)
    {
        if (term[loc_x] == 1 && term[loc_y] == 1 && term[loc_z] == 1)
        {
            term.reset(loc_x);
            term.reset(loc_y);
            term.reset(loc_z);
            if (term.count() == 0)
                term.set(loc_1);
            h.push_back(term);
        }
        else if (term[loc_x] == 0 && term[loc_y] == 1 && term[loc_z] == 1)
        {
            term.reset(loc_y);
            term.reset(loc_z);
            if (term.count() == 0)
                term.set(loc_1);
            g.push_back(term);
        }
        else if (term[loc_x] == 1 && term[loc_y] == 0 && term[loc_z] == 1)
        {
            term.reset(loc_x);
            term.reset(loc_z);
            if (term.count() == 0)
                term.set(loc_1);
            f.push_back(term);
        }
        else if (term[loc_x] == 1 && term[loc_y] == 1 && term[loc_z] == 0)
        {
            term.reset(loc_x);
            term.reset(loc_y);
            if (term.count() == 0)
                term.set(loc_1);
            e.push_back(term);
        }
        else if (term[loc_x] == 0 && term[loc_y] == 0 && term[loc_z] == 1)
        {
            term.reset(loc_z);
            if (term.count() == 0)
                term.set(loc_1);
            d.push_back(term);
        }
        else if (term[loc_x] == 0 && term[loc_y] == 1 && term[loc_z] == 0)
        {
            term.reset(loc_y);
            if (term.count() == 0)
                term.set(loc_1);
            c.push_back(term);
        }
        else if (term[loc_x] == 1 && term[loc_y] == 0 && term[loc_z] == 0)
        {
            term.reset(loc_x);
            if (term.count() == 0)
                term.set(loc_1);
            b.push_back(term);
        }
        else
            a.push_back(term);
    }
}
void ProcessingDATF1(int r, vector<Poly>& s, vector<Poly>& A, vector<Poly>& B, vector<Poly>& C, vector<Poly>& D, vector<Poly>& E, vector<Poly>& F, vector<Poly>& G, vector<Poly>& H)
{
    int i;
    vector<Poly> resultB, resultC, resultD, resultE, resultF, resultG, resultH;
    for (i = 0; i < 320; i++)
    {
        DATF(s[i], A[r * 320 + i], B[r * 320 + i], C[r * 320 + i], D[r * 320 + i], E[r * 320 + i], F[r * 320 + i], G[r * 320 + i], H[r * 320 + i]);
        if (B[r * 320 + i].empty() == false)
            resultB.push_back(B[r * 320 + i]);
        if (C[r * 320 + i].empty() == false)
            resultC.push_back(C[r * 320 + i]);
        if (D[r * 320 + i].empty() == false)
            resultD.push_back(D[r * 320 + i]);
        if (E[r * 320 + i].empty() == false)
            resultE.push_back(E[r * 320 + i]);
        if (F[r * 320 + i].empty() == false)
            resultF.push_back(F[r * 320 + i]);
        if (G[r * 320 + i].empty() == false)
            resultG.push_back(G[r * 320 + i]);
        if (H[r * 320 + i].empty() == false)
            resultH.push_back(H[r * 320 + i]);


    }


    vector<Poly> zeroized_polysB = zeroizePolynomials(resultB);
    vector<Poly> zeroized_polysC = zeroizePolynomials(resultC);
    vector<Poly> zeroized_polysD = zeroizePolynomials(resultD);
    vector<Poly> zeroized_polysE = zeroizePolynomials(resultE);
    vector<Poly> zeroized_polysF = zeroizePolynomials(resultF);
    vector<Poly> zeroized_polysG = zeroizePolynomials(resultG);
    vector<Poly> zeroized_polysH = zeroizePolynomials(resultH);


    cout << "B  Zeroized polys: " << endl;
    for (auto& poly : zeroized_polysB)
        printPoly(poly);
    cout << "C  Zeroized polys: " << endl;
    for (auto& poly : zeroized_polysC)
        printPoly(poly);
    cout << "D  Zeroized polys: " << endl;
    for (auto& poly : zeroized_polysD)
        printPoly(poly);
    cout << "E  Zeroized polys: " << endl;
    for (auto& poly : zeroized_polysE)
        printPoly(poly);
    cout << "F  Zeroized polys: " << endl;
    for (auto& poly : zeroized_polysF)
        printPoly(poly);
    cout << "G  Zeroized polys: " << endl;
    for (auto& poly : zeroized_polysG)
        printPoly(poly);
    cout << "H  Zeroized polys: " << endl;
    for (auto& poly : zeroized_polysH)
        printPoly(poly);


    int flag;
    Poly poly;
    PolyTerm term;

    for (i = 0; i < 320; i++)
    {
        poly.clear();
        if (isReplace(A[r * 320 + i]))
        {
            term.reset();
            term.set(loc_1 + 1 + 320 * r + i);
            poly.push_back(term);
        }
        else
            poly = A[r * 320 + i];

        flag = Judge_Poly(B[r * 320 + i]);
        if (flag >= -1)
            flag = zeroize(B[r * 320 + i], zeroized_polysB);
        if (flag == -2)
        {
            term.reset();
            term.set(loc_x);
            poly.push_back(term);
        }

        flag = Judge_Poly(C[r * 320 + i]);
        if (flag >= -1)
            flag = zeroize(C[r * 320 + i], zeroized_polysC);
        if (flag == -2)
        {
            term.reset();
            term.set(loc_y);
            poly.push_back(term);
        }

        flag = Judge_Poly(D[r * 320 + i]);
        if (flag >= -1)
            flag = zeroize(D[r * 320 + i], zeroized_polysD);
        if (flag == -2)
        {
            term.reset();
            term.set(loc_z);
            poly.push_back(term);
        }

        flag = Judge_Poly(E[r * 320 + i]);
        if (flag >= -1)
            flag = zeroize(E[r * 320 + i], zeroized_polysE);
        if (flag == -2)
        {
            term.reset();
            term.set(loc_x);
            term.set(loc_y);
            poly.push_back(term);
        }

        flag = Judge_Poly(F[r * 320 + i]);
        if (flag >= -1)
            flag = zeroize(F[r * 320 + i], zeroized_polysF);
        if (flag == -2)
        {
            term.reset();
            term.set(loc_x);
            term.set(loc_z);
            poly.push_back(term);
        }


        flag = Judge_Poly(G[r * 320 + i]);
        if (flag >= -1)
            flag = zeroize(G[r * 320 + i], zeroized_polysG);
        if (flag == -2)
        {
            term.reset();
            term.set(loc_y);
            term.set(loc_z);
            poly.push_back(term);
        }

        flag = Judge_Poly(H[r * 320 + i]);
        if (flag >= -1)
            flag = zeroize(H[r * 320 + i], zeroized_polysH);
        if (flag == -2)
        {
            term.reset();
            term.set(loc_x);
            term.set(loc_y);
            term.set(loc_z);
            poly.push_back(term);
        }


        s[i] = poly;
    }
}


void ProcessingDATF2(int r, vector<Poly>& s, vector<Poly>& A, vector<Poly>& B, vector<Poly>& C, vector<Poly>& D, vector<Poly>& E, vector<Poly>& F, vector<Poly>& G, vector<Poly>& H)
{
    int flag;
    Poly poly;
    PolyTerm term;

    map<Poly, int, PolyCompare> polyToVarB, polyToVarC, polyToVarD, polyToVarE, polyToVarF, polyToVarG, polyToVarH;

    for (int i = 0; i < 320; i++)
    {
        if (r >= 4)
            cout << i << ',';

        DATF(s[i], A[r * 320 + i], B[r * 320 + i], C[r * 320 + i], D[r * 320 + i], E[r * 320 + i], F[r * 320 + i], G[r * 320 + i], H[r * 320 + i]);

        poly.clear();
        if (isReplace(A[r * 320 + i]))
        {
            term.reset();
            term.set(loc_1 + 1 + 320 * r + i);
            poly.push_back(term);
        }
        else
            poly = A[r * 320 + i];


        term.reset();
        term.set(loc_x);
        if (isReplace(B[r * 320 + i]))
        {
            auto it = polyToVarB.find(B[r * 320 + i]);
            if (it != polyToVarB.end())
                term.set(polyToVarB[B[r * 320 + i]]);
            else
            {
                bool hasInverse = false;
                for (auto& pair : polyToVarB)
                {
                    if (isInverse(pair.first, B[r * 320 + i]))
                    {
                        term.set(polyToVarB[pair.first]);
                        poly.push_back(term);
                        term.reset();
                        term.set(loc_x);
                        hasInverse = true;
                        break;
                    }
                }
                if (!hasInverse)
                {
                    polyToVarB[B[r * 320 + i]] = loc_1 + 1 + 320 * round + 320 * r + i;
                    term.set(polyToVarB[B[r * 320 + i]]);
                }
            }
            poly.push_back(term);
        }
        else
        {
            if (B[r * 320 + i].empty() == false)
            for (auto& t : B[r * 320 + i])
            {
                if (t[loc_1] == 1)
                    t.reset(loc_1);
                t.set(loc_x);
                poly.push_back(t);
            }
        }


        term.reset();
        term.set(loc_y);
        if (isReplace(C[r * 320 + i]))
        {
            auto it = polyToVarC.find(C[r * 320 + i]);
            if (it != polyToVarC.end())
                term.set(polyToVarC[C[r * 320 + i]]);
            else
            {
                bool hasInverse = false;
                for (auto& pair : polyToVarC)
                {
                    if (isInverse(pair.first, C[r * 320 + i]))
                    {
                        term.set(polyToVarC[pair.first]);
                        poly.push_back(term);
                        term.reset();
                        term.set(loc_y);
                        hasInverse = true;
                        break;
                    }
                }
                if (!hasInverse)
                {
                    polyToVarC[C[r * 320 + i]] = loc_1 + 1 + 2 * 320 * round + 320 * r + i;
                    term.set(polyToVarC[C[r * 320 + i]]);
                }
            }
            poly.push_back(term);
        }
        else
        {
            if (C[r * 320 + i].empty() == false)
            for (auto& t : C[r * 320 + i])
            {
                if (t[loc_1] == 1)
                    t.reset(loc_1);
                t.set(loc_y);
                poly.push_back(t);
            }
        }



        term.reset();
        term.set(loc_z);
        if (isReplace(D[r * 320 + i]))
        {
            auto it = polyToVarD.find(D[r * 320 + i]);
            if (it != polyToVarD.end())
                term.set(polyToVarD[D[r * 320 + i]]);
            else
            {
                bool hasInverse = false;
                for (auto& pair : polyToVarD)
                {
                    if (isInverse(pair.first, D[r * 320 + i]))
                    {
                        term.set(polyToVarD[pair.first]);
                        poly.push_back(term);
                        term.reset();
                        term.set(loc_z);
                        hasInverse = true;
                        break;
                    }
                }
                if (!hasInverse)
                {
                    polyToVarD[D[r * 320 + i]] = loc_1 + 1 + 3 * 320 * round + 320 * r + i;
                    term.set(polyToVarD[D[r * 320 + i]]);
                }
            }
            poly.push_back(term);
        }
        else
        {
            if (D[r * 320 + i].empty() == false)
            {
                for (auto& t : D[r * 320 + i])
                {
                    if (t[loc_1] == 1)
                        t.reset(loc_1);
                    t.set(loc_z);
                    poly.push_back(t);
                }
            }

        }


        term.reset();
        term.set(loc_x);
        term.set(loc_y);
        if (isReplace(E[r * 320 + i]))
        {
            auto it = polyToVarE.find(E[r * 320 + i]);
            if (it != polyToVarE.end())
                term.set(polyToVarE[E[r * 320 + i]]);
            else
            {
                bool hasInverse = false;
                for (auto& pair : polyToVarE)
                {
                    if (isInverse(pair.first, E[r * 320 + i]))
                    {
                        term.set(polyToVarE[pair.first]);
                        poly.push_back(term);
                        term.reset();
                        term.set(loc_x);
                        term.set(loc_y);
                        hasInverse = true;
                        break;
                    }
                }
                if (!hasInverse)
                {
                    polyToVarE[E[r * 320 + i]] = loc_1 + 1 + 4 * 320 * round + 320 * r + i;
                    term.set(polyToVarE[E[r * 320 + i]]);
                }
            }
            poly.push_back(term);
        }
        else
        {
            if (E[r * 320 + i].empty() == false)
                for (auto& t : E[r * 320 + i])
                {
                    if (t[loc_1] == 1)
                        t.reset(loc_1);
                    t.set(loc_x);
                    t.set(loc_y);
                    poly.push_back(t);
                }
        }

        term.reset();
        term.set(loc_x);
        term.set(loc_z);
        if (isReplace(F[r * 320 + i]))
        {
            auto it = polyToVarF.find(F[r * 320 + i]);
            if (it != polyToVarF.end())
                term.set(polyToVarF[F[r * 320 + i]]);
            else
            {
                bool hasInverse = false;
                for (auto& pair : polyToVarF)
                {
                    if (isInverse(pair.first, F[r * 320 + i]))
                    {
                        term.set(polyToVarF[pair.first]);
                        poly.push_back(term);
                        term.reset();
                        term.set(loc_x);
                        term.set(loc_z);
                        hasInverse = true;
                        break;
                    }
                }
                if (!hasInverse)
                {
                    polyToVarF[F[r * 320 + i]] = loc_1 + 1 + 5 * 320 * round + 320 * r + i;
                    term.set(polyToVarF[F[r * 320 + i]]);
                }
            }
            poly.push_back(term);
        }
        else
        {
            if (F[r * 320 + i].empty() == false)
                for (auto& t : F[r * 320 + i])
                {
                    if (t[loc_1] == 1)
                        t.reset(loc_1);
                    t.set(loc_x);
                    t.set(loc_z);
                    poly.push_back(t);
                }
        }


        term.reset();
        term.set(loc_y);
        term.set(loc_z);
        if (isReplace(G[r * 320 + i]))
        {
            auto it = polyToVarG.find(G[r * 320 + i]);
            if (it != polyToVarG.end())
                term.set(polyToVarG[G[r * 320 + i]]);
            else
            {
                bool hasInverse = false;
                for (auto& pair : polyToVarG)
                {
                    if (isInverse(pair.first, G[r * 320 + i]))
                    {
                        term.set(polyToVarG[pair.first]);
                        poly.push_back(term);
                        term.reset();
                        term.set(loc_y);
                        term.set(loc_z);
                        hasInverse = true;
                        break;
                    }
                }
                if (!hasInverse)
                {
                    polyToVarG[G[r * 320 + i]] = loc_1 + 1 + 6 * 320 * round + 320 * r + i;
                    term.set(polyToVarG[G[r * 320 + i]]);
                }
            }
            poly.push_back(term);
        }
        else
        {
            if (G[r * 320 + i].empty() == false)
                for (auto& t : G[r * 320 + i])
                {
                    if (t[loc_1] == 1)
                        t.reset(loc_1);
                    t.set(loc_y);
                    t.set(loc_z);
                    poly.push_back(t);
                }
        }


        term.reset();
        term.set(loc_x);
        term.set(loc_y);
        term.set(loc_z);
        if (isReplace(H[r * 320 + i]))
        {
            auto it = polyToVarH.find(H[r * 320 + i]);
            if (it != polyToVarH.end())
                term.set(polyToVarH[H[r * 320 + i]]);
            else
            {
                bool hasInverse = false;
                for (auto& pair : polyToVarH)
                {
                    if (isInverse(pair.first, H[r * 320 + i]))
                    {
                        term.set(polyToVarH[pair.first]);
                        poly.push_back(term);
                        term.reset();
                        term.set(loc_x);
                        term.set(loc_y);
                        term.set(loc_z);
                        hasInverse = true;
                        break;
                    }
                }
                if (!hasInverse)
                {
                    polyToVarH[H[r * 320 + i]] = loc_1 + 1 + 7 * 320 * round + 320 * r + i;
                    term.set(polyToVarH[H[r * 320 + i]]);
                }
            }
            poly.push_back(term);
        }
        else
        {
            if (H[r * 320 + i].empty() == false)
                for (auto& t : H[r * 320 + i])
                {
                    if (t[loc_1] == 1)
                        t.reset(loc_1);
                    t.set(loc_x);
                    t.set(loc_y);
                    t.set(loc_z);
                    poly.push_back(t);
                }
        }


        s[i] = poly;
    }

    cout << endl;
}


Poly calculatedifference(Poly poly)
{
    Poly difference;
    for (auto& term : poly)
    {
        if (term[loc_x] == 1 && term[loc_y] == 1 && term[loc_z] == 1)
        {
            term.reset(loc_x);
            term.reset(loc_y);
            term.reset(loc_z);
            if (term.count() == 0)
                term.set(loc_1);
            difference.push_back(term);
        }
    }
    return difference;
}
vector<Poly> calculateANF(vector<int>& delta1, vector<int>& delta2, vector<int>& delta3, vector<Poly>& s, vector<Poly>& A, vector<Poly>& B, vector<Poly>& C, vector<Poly>& D, vector<Poly>& E, vector<Poly>& F, vector<Poly>& G, vector<Poly>& H)
{
    int i, j, k;
    bitset<128> IV1(0), IV2(0), IV3(0);
    for (i = 0; i < delta1.size(); i++)
        IV1.set(delta1[i]);
    for (i = 0; i < delta2.size(); i++)
        IV2.set(delta2[i]);
    for (i = 0; i < delta3.size(); i++)
        IV3.set(delta3[i]);



    Poly poly;
    PolyTerm term;
    term.set(loc_1);
    poly.push_back(term);

    for (i = 0; i < 64; i++)
    {
        if (i == 0 || i == 9 || i == 20 || i == 21 || i == 29 || i == 30)
            s[i] = poly;
    }

    vector<int> KEYSET0 = { 0,30,61,64,94,125 };
    bitset<128> KK(0);
    for (i = 0; i < KEYSET0.size(); i++)
        KK.set(KEYSET0[i]);

    for (i = 0; i < 128; i++)
    {
        if (KK[i]==0)
        {
            poly.clear();
            term.reset();
            term.set(i);
            poly.push_back(term);
            s[64 + i] = poly;
        }
    }


    for (i = 0; i < 128; i++)
    {
        poly.clear();
        term.reset();
        if (IV1[i] == 1)
            term.set(loc_x);
        else if (IV2[i] == 1)
            term.set(loc_y);
        else if (IV3[i] == 1)
            term.set(loc_z);
        else
            term.set(i + 128);
        poly.push_back(term);
        s[192 + i] = poly;
    }

    int constant[96] = { 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1 };
    int offsets[5][2] = { 19,28,61,39,1,6,10,17,7,41 };


    cout << "Compute the  0  round" << endl;
    // pc
    for (j = 0; j < 8; j = j + 1)
    {
        if (constant[j] == 1)
            s[184 + j] = add_one(s[184 + j]);
    }
    // ps
    //  x0 ^= x4; x4 ^= x3; x2 ^= x1;
    for (j = 0; j < 64; j++)
    {
        s[j] = add(s[j], s[64 * 4 + j]);
        s[64 * 4 + j] = add(s[64 * 4 + j], s[64 * 3 + j]);
        s[64 * 2 + j] = add(s[64 * 2 + j], s[64 * 1 + j]);
    }

    for (i = 1; i < round; i++)
    {
        vector<Poly> t0, t1, t2, t3, t4;
        //t0 = x0; t1 = x1; t2 = x2; t3 = x3; t4 = x4;
        //t0 = ~t0; t1 = ~t1; t2 = ~t2; t3 = ~t3; t4 = ~t4;
        //t0 &= x1; t1 &= x2; t2 &= x3; t3 &= x4; t4 &= x0;
        for (j = 0; j < 64; j++)
        {
            t0.push_back(multiply(add_one(s[j]), s[64 + j]));
            t1.push_back(multiply(add_one(s[64 + j]), s[64 * 2 + j]));
            t2.push_back(multiply(add_one(s[64 * 2 + j]), s[64 * 3 + j]));
            t3.push_back(multiply(add_one(s[64 * 3 + j]), s[64 * 4 + j]));
            t4.push_back(multiply(add_one(s[64 * 4 + j]), s[j]));
        }
        //x0 ^= t1; x1 ^= t2; x2 ^= t3; x3 ^= t4; x4 ^= t0;
        for (j = 0; j < 64; j++)
        {
            s[64 * 0 + j] = add(s[64 * 0 + j], t1[j]);
            s[64 * 1 + j] = add(s[64 * 1 + j], t2[j]);
            s[64 * 2 + j] = add(s[64 * 2 + j], t3[j]);
            s[64 * 3 + j] = add(s[64 * 3 + j], t4[j]);
            s[64 * 4 + j] = add(s[64 * 4 + j], t0[j]);
        }
        //x1 ^= x0; x0 ^= x4; x3 ^= x2; x2 = ~x2;
        for (j = 0; j < 64; j++)
        {
            s[64 + j] = add(s[j], s[64 + j]);
            s[j] = add(s[64 * 4 + j], s[j]);
            s[64 * 3 + j] = add(s[64 * 3 + j], s[64 * 2 + j]);
            s[64 * 2 + j] = add_one(s[64 * 2 + j]);
        }

        //cout << "pl" << endl;
        for (j = 0; j < 5; j++)
        {
            vector<Poly> t;
            for (k = 0; k < offsets[j][0]; k = k + 1)
                t.push_back(s[64 * j + k + 64 - offsets[j][0]]);
            for (k = offsets[j][0]; k < 64; k = k + 1)
                t.push_back(s[64 * j + k - offsets[j][0]]);

            for (k = 0; k < offsets[j][1]; k = k + 1)
                t[k] = add(t[k], s[64 * j + k + 64 - offsets[j][1]]);
            for (k = offsets[j][1]; k < 64; k = k + 1)
                t[k] = add(t[k], s[64 * j + k - offsets[j][1]]);

            for (k = 0; k < 64; k = k + 1)
                s[64 * j + k] = add(s[64 * j + k], t[k]);
        }

        cout << "Compute the  " << i << "  round" << endl;

        // pc
       // cout << "pc" << endl;
        for (j = 0; j < 8; j = j + 1)
        {
            if (constant[8 * i + j] == 1)
                s[184 + j] = add_one(s[184 + j]);
        }

        // ps
        //  x0 ^= x4; x4 ^= x3; x2 ^= x1;
        //cout << "ps" << endl;
        for (j = 0; j < 64; j++)
        {
            s[j] = add(s[j], s[64 * 4 + j]);
            s[64 * 4 + j] = add(s[64 * 4 + j], s[64 * 3 + j]);
            s[64 * 2 + j] = add(s[64 * 2 + j], s[64 * 1 + j]);
        }

        if (i - 1 < N0)
            ProcessingDATF1(i - 1, s, A, B, C, D, E, F, G, H);
        else
            ProcessingDATF2(i - 1, s, A, B, C, D, E, F, G, H);

       // for (j = 0; j < 320; j++)
      //      printPoly(s[j]);
       

    }
    vector<Poly> t0, t1, t2, t3, t4;
    //t0 = x0; t1 = x1; t2 = x2; t3 = x3; t4 = x4;
    //t0 = ~t0; t1 = ~t1; t2 = ~t2; t3 = ~t3; t4 = ~t4;
    //t0 &= x1; t1 &= x2; t2 &= x3; t3 &= x4; t4 &= x0;
    for (j = 0; j < 64; j++)
    {
        t0.push_back(multiply(add_one(s[j]), s[64 + j]));
        t1.push_back(multiply(add_one(s[64 + j]), s[64 * 2 + j]));
        t2.push_back(multiply(add_one(s[64 * 2 + j]), s[64 * 3 + j]));
        t3.push_back(multiply(add_one(s[64 * 3 + j]), s[64 * 4 + j]));
        t4.push_back(multiply(add_one(s[64 * 4 + j]), s[j]));
    }
    //x0 ^= t1; x1 ^= t2; x2 ^= t3; x3 ^= t4; x4 ^= t0;
    for (j = 0; j < 64; j++)
    {
        s[64 * 0 + j] = add(s[64 * 0 + j], t1[j]);
        s[64 * 1 + j] = add(s[64 * 1 + j], t2[j]);
        s[64 * 2 + j] = add(s[64 * 2 + j], t3[j]);
        s[64 * 3 + j] = add(s[64 * 3 + j], t4[j]);
        s[64 * 4 + j] = add(s[64 * 4 + j], t0[j]);
    }
    //x1 ^= x0; x0 ^= x4; x3 ^= x2; x2 = ~x2;
    for (j = 0; j < 64; j++)
    {
        s[64 + j] = add(s[j], s[64 + j]);
        s[j] = add(s[64 * 4 + j], s[j]);
        s[64 * 3 + j] = add(s[64 * 3 + j], s[64 * 2 + j]);
        s[64 * 2 + j] = add_one(s[64 * 2 + j]);
    }

    vector<Poly> difference;
    for (i = 0; i < 320; i++)
        difference.push_back(calculatedifference(s[i]));
    return difference;
}


//   Part 3: Evaluating the Bias
vector<Poly> calculateSeparate(Poly poly)
{
    vector<Poly> Separate;
    int i;
    Poly poly1;
    PolyTerm variable;

    int size = poly.size();
    int* flag = new int[size];
    for (i = 0; i < size; i++)
        flag[i] = 0;

    for (i = 0; i < size + 1; i++)
    {
        if (i == size)
        {
            if (poly1.size() == 0)
                break;
            Separate.push_back(poly1);
            poly1.clear();
            i = 0;
        }
        if (flag[i] == 0)
        {
            if (poly1.size() == 0)
            {
                poly1.push_back(poly[i]);
                variable = poly[i];
                flag[i] = 1;
            }
            else
            {
                if ((variable & poly[i]).any())
                {
                    poly1.push_back(poly[i]);
                    variable = variable | poly[i];
                    flag[i] = 1;
                    i = 0;
                }
            }
        }
    }
    delete[] flag;

    return Separate;
}
int cardinality(Poly poly)
{
    int i, maxx = 0;
    PolyTerm variable;
    vector<Poly> Separate = calculateSeparate(poly);
    for (i = 0; i < Separate.size(); i++)
    {
        variable.reset();
        for (auto& term : Separate[i])
            variable = variable | term;
        variable[loc_1] = 0;
        if (variable.count() > maxx)
            maxx = variable.count();
    }
    return maxx;
}

Poly simplify_f(int flag, Poly f, int value)
{
    Poly poly;
    if (value == 0)
    {
        for (const auto& term : f)
        {
            if (term[flag] == 0)
                poly.push_back(term);
        }

    }
    if (value == 1)
    {
        for (auto term : f)
        {
            if (term[flag] == 1)
            {
                term.reset(flag);
                if (term.count() == 0)
                    term.set(loc_1);
                poly.push_back(term);
            }
            else
                poly.push_back(term);
        }
        unordered_map<PolyTerm, int> occurrences;
        for (const auto& tt : poly)
            occurrences[tt]++;

        poly.clear();
        for (const auto& pair : occurrences)
            if (pair.second % 2 == 1)
                poly.push_back(pair.first);
    }
    return poly;
}
int calculatevalue(Poly poly, PolyTerm term, vector<int> variables, int size)
{
    int value = 0;
    int flag;
    for (const auto& tttt : poly)
    {
        if (tttt[loc_1] == 1)
            value = value ^ 1;
        else
        {
            for (int i = 0; i < size; i++)
            {
                if (tttt[variables[i]] == 1)
                {
                    if (term[variables[i]] == 1)
                        flag = 1;
                    else
                    {
                        flag = 0;
                        break;
                    }
                }
            }
            if (flag == 1)
                value = value ^ 1;
        }
    }
    return value;
}


double calculatebias_v1(Poly poly, vector<double>& B_bias, vector<double>& C_bias, vector<double>& D_bias, vector<double>& E_bias, vector<double>& F_bias, vector<double>& G_bias, vector<double>& H_bias)
{
    PolyTerm variable;
    for (const auto& term : poly)
        variable = variable | term;
    variable[loc_1] = 0;

    vector<int> variables;
    for (int i = 0; i < VARIABLE_COUNT; i++)
        if (variable[i] == 1)
            variables.push_back(i);
    int size = variables.size();


    double P0 = 0;
    for (long long j = 0; j < pow(2, size); j++)
    {
        PolyTerm term(0);
        for (int k = 0; k < size; k++)
            term[variables[k]] = (j >> k) & 1;
        if (calculatevalue(poly, term, variables, size) == 0)
        {
            double prob = 1.0;
            for (int k = 0; k < size; k++)
            {
                if (term[variables[k]] == 1)
                {
                    if (variables[k] < loc_1 + 1 + 320 * round)
                        prob = prob * 0.5;
                    else if (variables[k] < loc_1 + 1 + 2 * 320 * round)
                        prob = prob * (0.5 - B_bias[variables[k] - loc_1 - 1 - 320 * round]);
                    else if (variables[k] < loc_1 + 1 + 3 * 320 * round)
                        prob = prob * (0.5 - C_bias[variables[k] - loc_1 - 1 - 2 * 320 * round]);
                    else if (variables[k] < loc_1 + 1 + 4 * 320 * round)
                        prob = prob * (0.5 - D_bias[variables[k] - loc_1 - 1 - 3 * 320 * round]);
                    else if (variables[k] < loc_1 + 1 + 5 * 320 * round)
                        prob = prob * (0.5 - E_bias[variables[k] - loc_1 - 1 - 4 * 320 * round]);
                    else if (variables[k] < loc_1 + 1 + 6 * 320 * round)
                        prob = prob * (0.5 - F_bias[variables[k] - loc_1 - 1 - 5 * 320 * round]);
                    else if (variables[k] < loc_1 + 1 + 7 * 320 * round)
                        prob = prob * (0.5 - G_bias[variables[k] - loc_1 - 1 - 6 * 320 * round]);
                    else
                        prob = prob * (0.5 - H_bias[variables[k] - loc_1 - 1 - 7 * 320 * round]);
                }
                else
                {
                    if (variables[k] < loc_1 + 1 + 320 * round)
                        prob = prob * 0.5;
                    else if (variables[k] < loc_1 + 1 + 2 * 320 * round)
                        prob = prob * (0.5 + B_bias[variables[k] - loc_1 - 1 - 320 * round]);
                    else if (variables[k] < loc_1 + 1 + 3 * 320 * round)
                        prob = prob * (0.5 + C_bias[variables[k] - loc_1 - 1 - 2 * 320 * round]);
                    else if (variables[k] < loc_1 + 1 + 4 * 320 * round)
                        prob = prob * (0.5 + D_bias[variables[k] - loc_1 - 1 - 3 * 320 * round]);
                    else if (variables[k] < loc_1 + 1 + 5 * 320 * round)
                        prob = prob * (0.5 + E_bias[variables[k] - loc_1 - 1 - 4 * 320 * round]);
                    else if (variables[k] < loc_1 + 1 + 6 * 320 * round)
                        prob = prob * (0.5 + F_bias[variables[k] - loc_1 - 1 - 5 * 320 * round]);
                    else if (variables[k] < loc_1 + 1 + 7 * 320 * round)
                        prob = prob * (0.5 + G_bias[variables[k] - loc_1 - 1 - 6 * 320 * round]);
                    else
                        prob = prob * (0.5 + H_bias[variables[k] - loc_1 - 1 - 7 * 320 * round]);
                }
            }
            P0 = P0 + prob;
        }
    }

    return P0 - 0.5;
}
int simplifiedcalculatebias(Poly poly, vector<double>& B_bias, vector<double>& C_bias, vector<double>& D_bias, vector<double>& E_bias, vector<double>& F_bias, vector<double>& G_bias, vector<double>& H_bias)
{
    vector<Poly> Separate = calculateSeparate(poly);

    for (int i = 0; i < Separate.size(); i++)
    {
        PolyTerm variable;
        for (const auto& term : Separate[i])
            variable = variable | term;
        variable[loc_1] = 0;

        if (variable.count() < 5)
            if (calculatebias_v1(Separate[i], B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias) == 0)
                return 0;
    }
    return 1;
}
double preprocess(Poly& poly, vector<double>& B_bias, vector<double>& C_bias, vector<double>& D_bias, vector<double>& E_bias, vector<double>& F_bias, vector<double>& G_bias, vector<double>& H_bias)
{
    //cout << "Pretreat." << endl;
    double prob = 1.0;
    int flag = 1;
    while (flag == 1)
    {
        flag = 0;
        PolyTerm variable;
        for (const auto& term : poly)
            variable = variable | term;
        variable[loc_1] = 0;

        Poly f0, f1;
        double b0, b1;
        for (int loc = 0; loc < VARIABLE_COUNT; loc++)
        {
            if (variable[loc] == 1)
            {
                f0 = simplify_f(loc, poly, 0);
                f1 = simplify_f(loc, poly, 1);
                b0 = simplifiedcalculatebias(f0, B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias);
                b1 = simplifiedcalculatebias(f1, B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias);
                if (b0 == 0 && b1 == 0)
                    return -1;
                else if (b0 == 0)
                {
                    poly = f1;


                    if (loc < loc_1 + 1 + 320 * round)
                        prob = prob * 0.5;
                    else if (loc < loc_1 + 1 + 2 * 320 * round)
                        prob = prob * (0.5 - B_bias[loc - loc_1 - 1 - 320 * round]);
                    else if (loc < loc_1 + 1 + 3 * 320 * round)
                        prob = prob * (0.5 - C_bias[loc - loc_1 - 1 - 2 * 320 * round]);
                    else if (loc < loc_1 + 1 + 4 * 320 * round)
                        prob = prob * (0.5 - D_bias[loc - loc_1 - 1 - 3 * 320 * round]);
                    else if (loc < loc_1 + 1 + 5 * 320 * round)
                        prob = prob * (0.5 - E_bias[loc - loc_1 - 1 - 4 * 320 * round]);
                    else if (loc < loc_1 + 1 + 6 * 320 * round)
                        prob = prob * (0.5 - F_bias[loc - loc_1 - 1 - 5 * 320 * round]);
                    else if (loc < loc_1 + 1 + 7 * 320 * round)
                        prob = prob * (0.5 - G_bias[loc - loc_1 - 1 - 6 * 320 * round]);
                    else
                        prob = prob * (0.5 - H_bias[loc - loc_1 - 1 - 7 * 320 * round]);



                    flag = 1;
                    break;
                }
                else if (b1 == 0)
                {
                    poly = f0;
                    if (loc < loc_1 + 1 + 320 * round)
                        prob = prob * 0.5;
                    else if (loc < loc_1 + 1 + 2 * 320 * round)
                        prob = prob * (0.5 + B_bias[loc - loc_1 - 1 - 320 * round]);
                    else if (loc < loc_1 + 1 + 3 * 320 * round)
                        prob = prob * (0.5 + C_bias[loc - loc_1 - 1 - 2 * 320 * round]);
                    else if (loc < loc_1 + 1 + 4 * 320 * round)
                        prob = prob * (0.5 + D_bias[loc - loc_1 - 1 - 3 * 320 * round]);
                    else if (loc < loc_1 + 1 + 5 * 320 * round)
                        prob = prob * (0.5 + E_bias[loc - loc_1 - 1 - 4 * 320 * round]);
                    else if (loc < loc_1 + 1 + 6 * 320 * round)
                        prob = prob * (0.5 + F_bias[loc - loc_1 - 1 - 5 * 320 * round]);
                    else if (loc < loc_1 + 1 + 7 * 320 * round)
                        prob = prob * (0.5 + G_bias[loc - loc_1 - 1 - 6 * 320 * round]);
                    else
                        prob = prob * (0.5 + H_bias[loc - loc_1 - 1 - 7 * 320 * round]);
                    flag = 1;
                    break;
                }
            }
        }
    }
    return prob;
}
double calculatebias_v2(Poly poly, vector<double>& B_bias, vector<double>& C_bias, vector<double>& D_bias, vector<double>& E_bias, vector<double>& F_bias, vector<double>& G_bias, vector<double>& H_bias)
{
    double bias;

    vector<Poly> Separate = calculateSeparate(poly);

    double epsilon = 0.5;
    for (int i = 0; i < Separate.size(); i++)
    {
        PolyTerm variable;
        for (const auto& term : Separate[i])
            variable = variable | term;
        variable[loc_1] = 0;

        if (variable.count() <= 10)
            epsilon = 2 * epsilon * calculatebias_v1(Separate[i], B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias);   //Exhaustive computation
        else
        {
            double preprocess_p = preprocess(Separate[i], B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias);
            if (preprocess_p >= 0)
            {
                if (Separate[i].size() == 1 && Separate[i][0][loc_1] == 1)
                    bias = -0.5;
                else
                {
                    variable.reset();
                    for (const auto& term : Separate[i])
                        variable = variable | term;
                    variable[loc_1] = 0;

                    int minn = 200, min_temp, loc;
                    Poly f0, f1, h0, h1;
                    for (int j = 0; j < VARIABLE_COUNT; j++)
                    {
                        if (variable[j] == 1)
                        {
                            f0 = simplify_f(j, Separate[i], 0);
                            f1 = simplify_f(j, Separate[i], 1);
                            min_temp = max(cardinality(f0), cardinality(f1));
                            if (min_temp < minn)
                            {
                                minn = min_temp;
                                h0 = f0;
                                h1 = f1;
                                loc = j;
                            }
                        }
                    }
                    double p_0;
                    if (loc < loc_1 + 1 + 320 * round)
                        p_0 = 0.5;
                    else if (loc < loc_1 + 1 + 2 * 320 * round)
                        p_0 = 0.5 + B_bias[loc - loc_1 - 1 - 320 * round];
                    else if (loc < loc_1 + 1 + 3 * 384 * round)
                        p_0 = 0.5 + C_bias[loc - loc_1 - 1 - 2 * 320 * round];
                    else if (loc < loc_1 + 1 + 4 * 384 * round)
                        p_0 = 0.5 + D_bias[loc - loc_1 - 1 - 3 * 320 * round];
                    else if (loc < loc_1 + 1 + 5 * 384 * round)
                        p_0 = 0.5 + E_bias[loc - loc_1 - 1 - 4 * 320 * round];
                    else if (loc < loc_1 + 1 + 6 * 384 * round)
                        p_0 = 0.5 + F_bias[loc - loc_1 - 1 - 5 * 320 * round];
                    else if (loc < loc_1 + 1 + 7 * 384 * round)
                        p_0 = 0.5 + G_bias[loc - loc_1 - 1 - 6 * 320 * round];
                    else
                        p_0 = 0.5 + H_bias[loc - loc_1 - 1 - 7 * 320 * round];




                    if (simplifiedcalculatebias(h0, B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias) == 1)
                    {
                        if (simplifiedcalculatebias(h1, B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias) == 1)
                            bias = p_0 * calculatebias_v2(h0, B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias) + (1 - p_0) * calculatebias_v2(h1, B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias);
                        else
                            bias = p_0 * (calculatebias_v2(h0, B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias));
                    }
                    else
                    {
                        if (simplifiedcalculatebias(h1, B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias) == 1)
                            bias = (1 - p_0) * (calculatebias_v2(h1, B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias));
                        else
                            bias = 0;
                    }
                }
                bias = preprocess_p * bias;
                epsilon = 2 * epsilon * bias;
            }
            else if (preprocess_p == -1)
            {
                bias = 0;
                epsilon = 0;
            }
        }
        if (epsilon == 0)
            return 0;
    }
    return epsilon;
}


void calculateBbias(vector<Poly> B, vector<Poly>& C, vector<Poly>& D, vector<Poly>& E, vector<Poly>& F, vector<Poly>& G, vector<Poly>& H, vector<double>& B_bias, vector<double>& C_bias, vector<double>& D_bias, vector<double>& E_bias, vector<double>& F_bias, vector<double>& G_bias, vector<double>& H_bias)
{
    for (int i = 0; i < 320 * round; i++)
    {
        if (B[i].size() == 0)
            B_bias[i] = 0.5;
        else if (B[i].size() == 1 && B[i][0][loc_1] == 1)
            B_bias[i] = -0.5;
        else
            B_bias[i] = calculatebias_v2(B[i], B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias);

        if (C[i].size() == 0)
            C_bias[i] = 0.5;
        else if (C[i].size() == 1 && C[i][0][loc_1] == 1)
            C_bias[i] = -0.5;
        else
            C_bias[i] = calculatebias_v2(C[i], B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias);

        if (D[i].size() == 0)
            D_bias[i] = 0.5;
        else if (D[i].size() == 1 && D[i][0][loc_1] == 1)
            D_bias[i] = -0.5;
        else
            D_bias[i] = calculatebias_v2(D[i], B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias);

        if (E[i].size() == 0)
            E_bias[i] = 0.5;
        else if (E[i].size() == 1 && E[i][0][loc_1] == 1)
            E_bias[i] = -0.5;
        else
            E_bias[i] = calculatebias_v2(E[i], B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias);


        if (F[i].size() == 0)
            F_bias[i] = 0.5;
        else if (F[i].size() == 1 && F[i][0][loc_1] == 1)
            F_bias[i] = -0.5;
        else
            F_bias[i] = calculatebias_v2(F[i], B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias);

        if (G[i].size() == 0)
            G_bias[i] = 0.5;
        else if (G[i].size() == 1 && G[i][0][loc_1] == 1)
            G_bias[i] = -0.5;
        else
            G_bias[i] = calculatebias_v2(G[i], B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias);

        if (H[i].size() == 0)
            H_bias[i] = 0.5;
        else if (H[i].size() == 1 && H[i][0][loc_1] == 1)
            H_bias[i] = -0.5;
        else
            H_bias[i] = calculatebias_v2(H[i], B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias);


    }
}







void CHDL_distinguishers_6round()
{

    cout << "Calculate the bias of the CHDL distinguisher for 6-round Ascon, excluding linear layers." << endl;

    vector<int> delta1 = { 0,64 };
    vector<int> delta2 = { 30,64 + 30 };
    vector<int> delta3 = { 61,64 + 61 };


    vector<Poly> s(320);
    vector<Poly> A(320 * round), B(320 * round), C(320 * round), D(320 * round), E(320 * round), F(320 * round), G(320 * round), H(320 * round);
    vector<Poly> difference = calculateANF(delta1, delta2, delta3, s, A, B, C, D, E, F, G, H);
    cout << "The DATF has been calculated." << endl << endl;


    vector<double> B_bias(320 * round), C_bias(320 * round), D_bias(320 * round), E_bias(320 * round), F_bias(320 * round), G_bias(320 * round), H_bias(320 * round);
    calculateBbias(B, C, D, E, F, G, H, B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias);


    clock_t start, end;
    for (int i = 0; i < 64; i++)
    {
        start = clock();
        double bias = calculatebias_v2(difference[i], B_bias, C_bias, D_bias, E_bias, F_bias, G_bias, H_bias);
        end = clock();
        printPoly(difference[i]);
        cout << i << "   bias: " << bias;
        cout << "     time = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
    }

}




int main()
{

    CHDL_distinguishers_6round();
    system("pause");
    return 0;
}


