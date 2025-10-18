//
// Created by Andrea Cipriani on 20/9/25.
// A neural network works as a black box, we can guess how it'll work, but in the end it will choose how to work alone
// A network will be composed of several layers, each layer is made of neurons
// Each neuron will have a set of weights and biases to calculate its own value
// In the end the network will just set the input layer (I)
// done that the first layer (H1) will be calculated, then the second one (H2) and so on until it'll reach the output layer (O)
// Selecting the most probable output, we'll see if the guess it's correct

#ifndef NN_UTILS_H
#define NN_UTILS_H

#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>

enum LAYERS {         //enum for the last and first layer
    INPUT, OUTPUT
};
enum ALGO {           //enum for the normalizing algorithms
    NONE, SIGMOID, RELU, SOFTMAX
};

//! ONLY FOR TRAINED HIDDEN LAYERS
//Sigmoid function for normalizing activations
//Since the value can be any real number, the sigmoid function can map the value in this way:
//[-infty, +infty] -> [0, 1]
//The derivative of the sigmoid function has the global maximum in 0, and it tends to 0 to -infty and + infty.
std::vector<double> inline sigmoid(const std::vector<double>& value) {
    std::vector<double> result;
    result.reserve(value.size());
    for (const double v : value) {
        result.push_back(1/(1+std::exp(-(v))));
    }
    return result;
}
double inline sigmoid(const double& value) {
    return (1/(1+std::exp(-(value))));
}

//! ONLY FOR THE OUTPUT LAYER
//Softmax functions, normalizes the output making it a probability
//e.g, {1,2,3,4,1,2,3} becomes {0.024, 0.064, 0.175, 0.475, 0.024, 0.064, 0.175}
std::vector<double> inline softMax(const std::vector<double>& value) {
    std::vector<double> softmax;
    std::vector<double> expValue;
    double sum = 0;
    for (const double v : value) {
        const double temp = (std::exp(v - *std::ranges::max_element(value)));
        expValue.push_back(temp);
        sum += temp;
    }
    softmax.reserve(expValue.size());
    for (const double v : expValue) {
        softmax.push_back(v / sum);
    }
    return softmax;
};

//! ONLY FOR THE TRAINING OF THE HIDDEN LAYER
//ReLU is useful for the hidden layers' training
//Sigmoid function derivative for great values is really close to 0
//That means that if the perfect weight is 100 and the model has a 50, the error is practically 0
//ReLU keeps the error great
double inline reLU(const double& value) {
    return(std::max(0.0, value));
}
std::vector<double> inline reLU(const std::vector<double>& value) {
    std::vector<double> relu;
    relu.reserve(value.size());
    for (const double v : value) {
        relu.push_back(std::max(0.0, v));
    }
    return relu;
}

template <typename T>
inline std::vector<T> split(const std::string& string, const char delimiter = '\n') {
    std::string s;
    std::stringstream ss(string);
    std::vector<T> v;
    while (getline(ss, s, delimiter)) {
        std::stringstream converter(s);
        T item;
        if (converter >> item) {
            v.push_back(item);
        }
    }
    return v;
}

//Cost of the operation
inline double cost(const std::vector<double>& actualResult, const std::vector<double>& expectedResult, double multiplier = 1) {
    double sum = 0;
    if (actualResult.size() != expectedResult.size()) {
        std::cerr << "Size mismatch!" << std::endl;
    }
    for (int i = 0; i < actualResult.size(); i++) {
        sum += std::pow(actualResult[i]-expectedResult[i], 2);
    }
    return multiplier * sum;
}

//Cross-entropy of the operation
inline double crossEntropy(const std::vector<double>& actualResult, const std::vector<double>& expectedResult) {
    double sum = 0;
    if (actualResult.size() != expectedResult.size()) {
        std::cerr << "Size mismatch!" << std::endl;
    }
    for (int i = 0; i < actualResult.size(); i++) {
        sum -= expectedResult[i] * std::log(actualResult[i]);
    }
    return sum;
}

//Difference between the actual vector and the expected one
inline std::vector<double> vectDiff (const std::vector<double>& actualVector, const std::vector<double>& expectedVector) {
    std::vector<double> res;
    res.reserve(actualVector.size());
    for (int i = 0; i < actualVector.size(); i++) {
        res.push_back(actualVector[i] - expectedVector[i]);
    }
    return res;
}

//Random function to generate the initial weights and biases of the model
class RandomDouble {
    std::mt19937_64 gen;
    std::uniform_real_distribution<double> dist;
public:
    RandomDouble(const double low, const double high) : gen(std::random_device{}()), dist(low, high) {}
    double operator()() {
        return dist(gen);
    }
};

//Single neuron structure
//Basically, a neuron is a function composed of several weights (w) and biases (b)
//To calculate the activation (a_n) of the neuron the formula is:
//\sum_{n = 0}^{#neurons of precedent layer} w_n * a_{n-1} + b_n
//If the neuron is part of the input layer we cannot calculate the activation, but we can set it
//In number recognition neural network, each pixel will become an input neuron where 0 = pure black and 1 = pure white
class Neuron {
    bool verbose;
    std::vector<double> weights;        //Weights of the arches
    std::vector<double> biases;         //Biases of the arches
    double value = NULL;                //Actual value
    double normalizedValue = NULL;      //Normalized value
public:

    //Constructor
    explicit Neuron(const int previousLayerSize, const bool v = false) : verbose(v) {
        //Each neuron is connected to all the neurons of the previous layer
        if (verbose) {
            std::cout << "(Ne) Creating neuron with " << previousLayerSize << " connections" << std::endl;
        }
        RandomDouble rd{-1.0, 1.0};
        for (int i = 0; i < previousLayerSize; i++) {
            weights.push_back(rd());   //Initially weights and biases are totally random
            biases.push_back(rd());
        }
        if (verbose) {
            for (int i = 0; i < weights.size(); i++) {
                std::cout << "Weight " << i << ": " << weights[i] << std::endl;
            }
            for (int i = 0; i < biases.size(); i++) {
                std::cout << "Bias " << i << ": " << biases[i] << std::endl;
            }
            std::cout << "(Ne) Total size: " << weights.size() << std::endl;
        }
    }

    //Calculate the neuron
    double calculate(const std::vector<double>& previousLayer, const ALGO algorithm = ALGO::NONE) {
        if (verbose) {
            std::cout << "Calculating..." << std::endl;
        }
        value = 0;
        for (int i = 0; i < previousLayer.size(); i++) {        //Calculate the activation values
            value += weights[i] * previousLayer[i] + biases[i];
            if (verbose) {
                std::cout << "Neuron value " << i << ": " << value << std::endl;
                std::cout << "Weight " << weights[i] << ", bias " << biases[i] << ", neuron: " << previousLayer[i] << std::endl;
            }
        }
        switch (algorithm) {
            case ALGO::SIGMOID:
                normalizedValue = sigmoid(value);
                break;
            case ALGO::RELU:
                normalizedValue = reLU(value);
                break;
            default:
                normalizedValue = value;
        }
        return normalizedValue;
    }
    double setValue(const double& newValue, const ALGO algorithm = ALGO::NONE) {           //Set a new value to the neuron (used only for the input)
        const double oldValue = value;
        value = newValue;
        switch (algorithm) {
            case ALGO::SIGMOID:
                normalizedValue = sigmoid(value);
                break;
            case ALGO::RELU:
                normalizedValue = reLU(value);
                break;
            default:
                normalizedValue = value;
        }
        return oldValue;
    }

    //Edit the neuron
    void setWeights(const std::vector<double>& newWeights) {
        weights = newWeights;                                  //Update the weights
    }
    void setBiases(const std::vector<double>& newBiases) {
        biases = newBiases;                                    //Update the biases
    }

    //Metadata of the neuron
    [[nodiscard]] std::vector<double> getWeights() const {
        return weights;                                     //Return the weights
    }
    [[nodiscard]] std::vector<double> getBiases() const {
        return biases;                                      //Return the biases
    }
    [[nodiscard]] double getValue(const bool normalized) const {
        if (normalized) return normalizedValue;                     //Return the value of the neuron
        return value;
    }
};

//A layer is a group of neuron with the same depth in the network
//The final network's structure will be like this:
//INPUT-H1-H2-...-Hn-OUTPUT
//The layer must be able to:
// * Edit the input neurons
// * Calculate the value of the hidden's and the output's layer
// * Select the neuron with the greatest value
// The last part is vital for the back-propagation and select the guess of the network
class Layer {
    bool verbose;
    int neuronsInLayer;        //Number of neurons in the layer
    std::vector<Neuron> neurons;                    //Vector with the neurons
public:
    //Constructor
    explicit Layer(const int numberOfNeurons, const Layer* previousLayer = nullptr, const bool v = false) : verbose(v), neuronsInLayer(numberOfNeurons) {
        if (verbose) {
            std::cout << "(Ly) Creating Layer with size " << neuronsInLayer << std::endl;
        }
        if (previousLayer) {
            if (verbose) {
                std::cout << "(Ly) Connecting to previous layer" << std::endl;
            }
            int precedentLayerSize = previousLayer->getSize();
            for (int i = 0; i < neuronsInLayer; i++) {
                neurons.emplace_back(precedentLayerSize, verbose);
            }
        } else {
            for (int i = 0; i < neuronsInLayer; i++) {
                neurons.emplace_back(0, verbose);
            }
        }
    }

    //Edit the layer
    std::vector<double> calculate(const Layer& previousLayer, const ALGO algorithm = ALGO::NONE) {                    //calculate each neuron value
        std::vector<double> result;
        if (verbose) {
            std::cout << "(Ly) Calculating layer " << std::endl;
        }
        result.reserve(neurons.size());
        for (Neuron& neuron : neurons) {
            result.push_back(neuron.calculate(previousLayer.toVector(), algorithm));
        }
        return result;
    }
    void setValues(const std::vector<double> &inputs, const ALGO algorithm = ALGO::NONE) {
        if (verbose) {
            std::cout << "(Ly) Setting values" << std::endl;
        }
        for (int i = 0; i < neuronsInLayer; i++) {
            neurons[i].setValue(inputs[i], algorithm);
        }
    }
    void setWeights(const std::vector<double> &inputs) {
        if (verbose) {
            std::cout << "(Ly) Setting weights" << std::endl;
        }
        for (int i = 0; i < neuronsInLayer; i++) {
            neurons[i].setWeights(inputs);
        }
    }
    void setBiases(const std::vector<double> &inputs) {
        if (verbose) {
            std::cout << "(Ly) Setting biases" << std::endl;
        }
        for (int i = 0; i < neuronsInLayer; i++) {
            neurons[i].setBiases(inputs);
        }
    }

    //Metadata about the layer
    [[nodiscard]] int getSize() const {                         //Number of neurons in layer
        return neuronsInLayer;
    }
    [[nodiscard]] std::vector<double> toVector(const bool normalized = true) const {        //Convert the layer to a vector
        std::vector<double> result;
        result.reserve(neurons.size());
        for (const Neuron& neuron : neurons) {
            result.push_back(neuron.getValue(normalized));
        }
        return result;
    }
    [[nodiscard]] std::vector<double> getWeights() const {
        std::vector<double> weightsToReturn;
        for (const auto& e : neurons) {
            for (std::vector<double> temp = e.getWeights(); auto v : temp) {
                weightsToReturn.push_back(v);
            }
        }
        return weightsToReturn;
    }
    [[nodiscard]] std::vector<double> getBiases() const {
        std::vector<double> biasesToReturn;
        for (const auto& e : neurons) {
            for (std::vector<double> temp = e.getBiases(); auto v : temp) {
                biasesToReturn.push_back(v);
            }
        }
        return biasesToReturn;
    }
    [[nodiscard]] int guessed() const {                  //Returns the highest value between neurons, useful only for the output
        if (verbose) {
            std::cout << "(Ly) Searching max value" << std::endl;
        }
        double max = 0;
        int position = 0;
        int counter = 0;
        for (int i = 0; i < neurons.size(); i++) {
            if (neurons[i].getValue(true) > max) {
                max = neurons[i].getValue(true);
                position = i;
            }
            counter++ ;
        }
        if (verbose) {
            std::cout << "Result of the search," << counter << " value:" << position << ", index:" << max << std::endl;
        }
        return position;
    }
    void clear() {
        neurons.clear();
        neurons.shrink_to_fit();
        neuronsInLayer = 0;
    }
};

class Network {
    //Structure
    bool verbose;
    Layer input;
    std::vector<Layer> hidden;
    Layer output;

    //Names vector
    std::vector<std::vector<std::string>> names;
    int guess = -1;                                     //# of the chosen output neuron (-1 == no operation done)
    double probability = 0;                             //Probability of the choice
public:
    explicit Network(const bool v = false) : verbose(v), input(0), output(0) {
        if (verbose) std::cout << "(Nr) Creating the empty network" << std::endl;
    }
    Network(const int inputNeurons, const int hiddenNeurons, const int outputNeurons, const bool v = false) : verbose(v), input(Layer(inputNeurons, nullptr, v)), output(Layer(outputNeurons, nullptr, v))
    {
        if (verbose) std::cout << "(Nr) Creating the network" << std::endl;
        hidden.emplace_back(hiddenNeurons, &input, v);
        output = Layer(outputNeurons, &hidden[0], v);
    }
    Network(const int inputNeurons, const std::vector<int>& hiddenNeurons, const int outputNeurons, const bool v = false) : verbose(v), input(Layer(inputNeurons, nullptr, v)), output(Layer(outputNeurons, nullptr, v))
    {
        if (verbose) std::cout << "(Nr) Creating the network" << std::endl;

        // Create hidden layers and connect them
        for (int i = 0; i < static_cast<int>(hiddenNeurons.size()); i++) {
            if (verbose) {
                if (i == 0) std::cout << "(Nr) Connecting first hidden layer" << std::endl;
                else std::cout << "(Nr) Connecting hidden layer " << i << std::endl;
            }
            if (i == 0)
                hidden.emplace_back(hiddenNeurons[i], &input, verbose);
            else
                hidden.emplace_back(hiddenNeurons[i], &hidden[i-1], verbose);
        }

        // Connect output to the last hidden layer (if any)
        if (!hidden.empty())
            output = Layer(outputNeurons, &hidden.back(), verbose);
    }
    //Sets the name of visible layers (INPUT and OUTPUT)
    void setName(const LAYERS layer, const std::vector<std::string>& newNames) {
        const int idx = static_cast<int>(layer);

        if (layer == LAYERS::INPUT && newNames.size() != input.getSize()) return;
        if (layer == LAYERS::OUTPUT && newNames.size() != output.getSize()) return;

        if (names.size() < 2) names.resize(2); // ensure two slots exist
        names[idx] = newNames;
    }
    void train(const std::vector<double>& inputs, const int label, const double learningRate = 0.001) {
        std::vector<double> expectedOutput(output.getSize(), 0.0);
        expectedOutput[label] = 1.0;

        // Forward pass
        std::vector<double> result = calculate(inputs);

        // Delta for output layer
        std::vector<double> deltaOutput = vectDiff(result, expectedOutput);

        // Store delta for all layers
        std::vector<std::vector<double>> delta(hidden.size() + 1);
        delta[hidden.size()] = deltaOutput;

        // Backpropagation for hidden layers
        for (int i = static_cast<int>(hidden.size()) - 1; i >= 0; --i) {
            std::vector<double> z = hidden[i].toVector(false);
            std::vector<double> deriv(z.size());
            for (int k = 0; k < z.size(); ++k) {
                deriv[k] = (z[k] > 0.0) ? 1.0 : 0.0;  // ReLU derivative
            }

            std::vector<double> deltaHidden(hidden[i].getSize(), 0.0);
            const std::vector<double>& deltaNext = delta[i + 1];
            std::vector<double> weightsNext = (i == hidden.size() - 1) ? output.getWeights() : hidden[i + 1].getWeights();
            int neuronsNext = (i == hidden.size() - 1) ? output.getSize() : hidden[i + 1].getSize();
            int neuronsCurrent = hidden[i].getSize();

            for (int j = 0; j < neuronsCurrent; ++j) {
                double sum = 0.0;
                for (int k = 0; k < neuronsNext; ++k) {
                    sum += weightsNext[k * neuronsCurrent + j] * deltaNext[k];
                }
                deltaHidden[j] = sum * deriv[j];
            }
            delta[i] = deltaHidden;
        }

        // Update weights and biases for hidden layers
        for (int i = 0; i < hidden.size(); ++i) {
            std::vector<double> prevActivations = (i == 0) ? input.toVector() : hidden[i - 1].toVector();
            std::vector<double> oldWeights = hidden[i].getWeights();
            std::vector<double> oldBiases = hidden[i].getBiases();

            for (int j = 0; j < hidden[i].getSize(); ++j) {
                for (int k = 0; k < prevActivations.size(); ++k) {
                    oldWeights[j * prevActivations.size() + k] -= learningRate * delta[i][j] * prevActivations[k];
                }
                oldBiases[j] -= learningRate * delta[i][j];
            }

            hidden[i].setWeights(oldWeights);
            hidden[i].setBiases(oldBiases);
        }

        // Update weights and biases for output layer
        std::vector<double> prevActivations = hidden.back().toVector();
        std::vector<double> oldWeights = output.getWeights();
        std::vector<double> oldBiases = output.getBiases();

        for (int j = 0; j < output.getSize(); ++j) {
            for (int k = 0; k < prevActivations.size(); ++k) {
                oldWeights[j * prevActivations.size() + k] -= learningRate * deltaOutput[j] * prevActivations[k];
            }
            oldBiases[j] -= learningRate * deltaOutput[j];
        }

        output.setWeights(oldWeights);
        output.setBiases(oldBiases);
    }
    std::vector<double> calculate(const std::vector<double>& inputs) {
        if (inputs.size() != input.getSize()) {     //Wrong number of inputs;
            std::cerr << "Input size mismatch" << std::endl;
        }
        if (verbose) {
            std::cout << "(Nr) Setting the input " << std::endl;
        }
        input.setValues(inputs);                    //Sets the input
        if (verbose) {
            std::cout << "(Nr) Calculating layers " << std::endl;
        }
        for (int i = 0; i < hidden.size(); i++) {   //Calculate each hidden layer
            if (i == 0) {                           //If H1, use I
                hidden[i].calculate(input, ALGO::RELU);
            } else {                                //Else Hn uses H(n-1)
                hidden[i].calculate(hidden[i-1], ALGO::RELU);
            }
            if (verbose) {
                if (i == hidden.size()-1) {
                    std::cout << "(Nr) Calculating output layer " << std::endl;
                } else {
                    std::cout << "(Nr) Calculating layer " << i << std::endl;
                }
            }
        }
        //Result is the vector form of the network decision
        std::vector<double> result = softMax(output.calculate(hidden[hidden.size()-1]));

        //This one gets the most probable output and the probability
        guess = output.guessed();
        if (verbose) {
            std::cout << names[OUTPUT][guess] << " " << result[guess] << std::endl;
        }
        return result;
    }
    std::string result() {
        if (guess < 0) return "Run calculate() first";

        // If no output names were set, generate default ones
        if (names.size() < 2 || names[1].empty()) {
            if (verbose) std::cout << "Output has no names, creating default ones" << std::endl;
            std::vector<std::string> defaultNames;
            defaultNames.reserve(output.getSize());
            for (int i = 0; i < output.getSize(); i++) {
                defaultNames.push_back(std::to_string(i));
            }
            setName(LAYERS::OUTPUT, defaultNames);
        }

        const std::string outName = names[1][guess];
        return "Guessed " + outName + " with probability " + std::to_string(probability * 100) + "%";
    }

    void saveModel(std::string filename = "*") const {          //SAVE the model
        if (filename == "*") {
            time_t timestamp;
            time(&timestamp);
            filename = std::to_string(timestamp);
        }
        filename += ".model";
        std::ofstream file("../models/" + filename);
        if (!file.is_open()) {
            std::cerr << "Could not open file \"" << filename << std::endl;
            return;
        }
        std::cout << "Saving model to: " << std::filesystem::current_path() << "/" << filename << std::endl;
        file << "I" << std::endl;                           //Save input details
        file << input.getSize() << std::endl;
        for (int i = 0; i < hidden.size(); i++) {
            file << "H" << i << std::endl;                           //Save hidden details
            file << hidden[i].getSize() << std::endl;
            std::vector<double> weights = hidden[i].getWeights();
            std::vector<double> biases = hidden[i].getBiases();
            for (const auto e : weights) {
                file << e << " ";
            }
            file << std::endl;
            for (const auto e : biases) {
                file << e << " ";
            }
            file << std::endl;
        }

        file << "O" << std::endl;                           //Save output details
        file << output.getSize() << std::endl;
        const std::vector<double> weights = output.getWeights();
        const std::vector<double> biases = output.getBiases();
        for (const auto e : weights) {
            file << e << " ";
        }
        file << std::endl;
        for (const auto e : biases) {
            file << e << " ";
        }
        file.close();
    }
    void loadModel(const std::string& filename) {
        // Reset current network
        input.clear();
        hidden.clear();
        output.clear();

        std::ifstream file("../models/" + filename + ".model");
        if (!file.is_open()) {
            std::cerr << "Could not open file \"" << filename << ".model\"" << std::endl;
            return;
        }

        std::string line;
        int hiddenIndex = 0;

        while (std::getline(file, line)) {
            if (line == "I") {
                // Input layer
                std::getline(file, line);
                int size = std::stoi(line);
                input = Layer(size, nullptr, verbose);

            } else if (!line.empty() && line[0] == 'H') {
                // Hidden layer
                std::getline(file, line);
                int size = std::stoi(line);

                if (hiddenIndex == 0)
                    hidden.emplace_back(size, &input, verbose);
                else
                    hidden.emplace_back(size, &hidden.back(), verbose);

                // Read weights
                std::getline(file, line);
                auto w = split<double>(line, ' ');

                // Read biases
                std::getline(file, line);
                auto b = split<double>(line, ' ');

                hidden.back().setWeights(w);
                hidden.back().setBiases(b);
                hiddenIndex++;

            } else if (line == "O") {
                // Output layer
                std::getline(file, line);
                int size = std::stoi(line);
                output = Layer(size, hidden.empty() ? &input : &hidden.back(), verbose);

                // Read weights
                std::getline(file, line);
                auto w = split<double>(line, ' ');

                // Read biases
                std::getline(file, line);
                auto b = split<double>(line, ' ');

                output.setWeights(w);
                output.setBiases(b);
            }
        }
        file.close();
    }
    /*
    double TestAccuracy(const std::vector<std::pair<std::vector<double>, int>>& TestDataset) {
        double accuracy = 0;
        for (const auto& test : TestDataset) {
            calculate(test.first);
        }
        return accuracy;
    }
    */
};

#endif //NN_UTILS_H
