//
//  ModuleDebug.cpp
//  SUNA
//
//  Created by RHVT on 23/3/19.
//  Copyright © 2019 RHVT. All rights reserved.
//

#include "ModuleDebug.h"

ModuleDebug::ModuleDebug()
{
}

ModuleDebug::~ModuleDebug()
{
}

// FOR DEBUG PURPOSES
void ModuleDebug::printInformationFlowGraph(Module *mod, const char *filename)
{
    FILE *fp = fopen(filename,"w");
    
    fprintf(fp, "digraph G {\n node[shape = circle]\n");
    
    auto n = mod->n;
    auto c = mod->c;
    auto neuron_excitation = mod->neuron_excitation;
    auto neuron_state = mod->neuron_state;
    auto previous_neuron_state = mod->previous_neuron_state;
    auto max_neuron_id = mod->max_neuron_id;
    
    //writing the neurons
    int i;
    for (i = 0; n[i].id >= 0; ++i) {
#ifdef CONTINUOUS_PARAM
        double neuronS = neuron_state[i];
        double previousN = previous_neuron_state[i];
#else
        double neuronS = static_cast<double>(neuron_state[i]);
        double previousN = static_cast<double>(previous_neuron_state[i]);
#endif
        
        switch(n[i].type) {
            case CONTROL: {
                if (neuron_excitation[i] >= EXCITATION_THRESHOLD) {
                    fprintf(fp, "%d [label=\"%d_s%.1f_ps%.1f\",shape=diamond,color=blue]\n", i, i, neuronS, previousN);
                }
                else {
                    fprintf(fp, "%d [label=\"%d_s%.1f_ps%.1f\",shape=diamond,color=red]\n", i, i, neuronS, previousN);
                }
            }
                break;
            case INPUT_IDENTITY: {
                if (neuron_excitation[i] >= EXCITATION_THRESHOLD) {
                    fprintf(fp, "%d [label=\"%d_s%.1f_ps%.1f\",shape=doublecircle,color=blue]\n", i, i, neuronS, previousN);
                }
                else {
                    fprintf(fp, "%d [label=\"%d_s%.1f_ps%.1f\",shape=doublecircle,color=red]\n", i, i, neuronS, previousN);
                }
            }
                break;
            case OUTPUT_IDENTITY: {
                fprintf(fp, "%d [label=\"%d_s%.1f_ps%.1f\",shape=doublecircle, style=filled,fillcolor=gray]\n", i, i, neuronS, previousN);
            }
                break;
            default: {
                if (neuron_excitation[i] >= EXCITATION_THRESHOLD) {
                    fprintf(fp, "%d [label=\"%d_s%.1f_ps%.1f\",color=blue]\n", i, i, neuronS, previousN);
                }
                else {
                    fprintf(fp, "%d [label=\"%d_s%.1f_ps%.1f\",color=red]\n", i, i, neuronS, previousN);
                }
            }
                break;
        }
    }
    
    int counter = 1;
    
    for (i = 0; c[i].from_neuron_id >= 0; ++i) {
        if (c[i].neuro_modulation < 0) {
#ifdef CONTINUOUS_PARAM
            double cw = c[i].weight;
#else
            double cw = static_cast<double>(c[i].weight);
#endif
            //no neuro modulation
            fprintf(fp, "%d -> %d [label=\"%f\"]\n", c[i].from_neuron_id, c[i].to_neuron_id, cw);
        }
        else {
            //with neuro modulation
            fprintf(fp, "%d [shape=point, label=\"\"]\n", max_neuron_id + counter);
            fprintf(fp, "%d -> %d [style=dashed] \n", mod->neuronIdToDNAIndex(c[i].neuro_modulation), max_neuron_id + counter);
            fprintf(fp, "%d -> %d [dir=none]\n", mod->neuronIdToDNAIndex(c[i].from_neuron_id), max_neuron_id + counter);
            fprintf(fp, "%d -> %d \n", max_neuron_id + counter, mod->neuronIdToDNAIndex(c[i].to_neuron_id));
            counter++;
        }
    }
    
    fprintf(fp, "}\n");
    fclose(fp);
}

void ModuleDebug::printGraph(Module *mod, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    
    fprintf(fp, "digraph G {\n node[shape = circle]\n");
    
    //writing the neurons
    
    auto n = mod->n;
    auto c = mod->c;
    auto max_neuron_id = mod->max_neuron_id;
    
    int i;
    for (i = 0; n[i].id >= 0; ++i) {
        switch(n[i].type) {
                //enum{IDENTITY, SIGMOID, THRESHOLD, RANDOM, CONTROL, NUMBER_OF_NEURON_TYPES, INPUT_IDENTITY, OUTPUT_IDENTITY };
            case CONTROL: {
                fprintf(fp, "%d [label=\"%d_fire%d\",shape=diamond]\n", i, i, n[i].firing_rate);
            }
                break;
            case IDENTITY: {
                fprintf(fp, "%d [label=\"%d_fire%d\",style=filled, fillcolor=gold]\n", i, i, n[i].firing_rate);
            }
                break;
            case THRESHOLD: {
                fprintf(fp, "%d [label=\"%d_fire%d\",style=filled, fillcolor=darkgreen]\n", i, i, n[i].firing_rate);
            }
                break;
            case ACTIVATION: {
                fprintf(fp, "%d [label=\"%d_fire%d\",style=filled, fillcolor=lightskyblue]\n", i, i, n[i].firing_rate);
            }
                break;
            case RANDOM: {
                fprintf(fp, "%d [label=\"%d_fire%d\",style=filled, fillcolor=coral]\n", i, i, n[i].firing_rate);
            }
                break;
            case INPUT_IDENTITY: {
                fprintf(fp, "%d [label=\"%d_fire%d\",shape=doublecircle]\n", i, i, n[i].firing_rate);
            }
                break;
            case OUTPUT_IDENTITY: {
                fprintf(fp, "%d [label=\"%d_fire%d\",shape=doublecircle, style=filled, fillcolor=gray]\n", i, i, n[i].firing_rate);
            }
                break;
            default:    {
                fprintf(fp, "%d\n",i);
            }
                break;
        }
    }
    
    int counter = 1;
    
    for (i = 0; c[i].from_neuron_id >= 0; ++i) {
        if (c[i].neuro_modulation < 0) {
#ifdef CONTINUOUS_PARAM
            fprintf(fp, "%d -> %d [label=\"%f\"]\n", mod->neuronIdToDNAIndex(c[i].from_neuron_id), mod->neuronIdToDNAIndex(c[i].to_neuron_id), c[i].weight);
#else
            fprintf(fp, "%d -> %d [label=\"%i\"]\n", mod->neuronIdToDNAIndex(c[i].from_neuron_id), mod->neuronIdToDNAIndex(c[i].to_neuron_id), c[i].weight);
#endif
     
        }
        else {
            //with neuro modulation
            fprintf(fp, "%d [shape=point, label=\"\"]\n", max_neuron_id + counter);
            fprintf(fp, "%d -> %d [style=dashed] \n", mod->neuronIdToDNAIndex(c[i].neuro_modulation), max_neuron_id + counter);
            fprintf(fp, "%d -> %d [dir=none]\n", mod->neuronIdToDNAIndex(c[i].from_neuron_id), max_neuron_id + counter);
            fprintf(fp, "%d -> %d \n", max_neuron_id + counter, mod->neuronIdToDNAIndex(c[i].to_neuron_id));
            counter++;
        }
    }
    
    fprintf(fp, "}\n");
    fclose(fp);
}

void ModuleDebug::printVars(Module *mod)
{
    auto neuron_excitation = mod->neuron_excitation;
    auto neuron_state = mod->neuron_state;
    auto previous_neuron_state = mod->previous_neuron_state;
    auto max_neuron_id = mod->max_neuron_id;
    auto number_of_neurons = mod->number_of_neurons;
    auto allocated_space = mod->allocated_space;
    auto primer_list = mod->primer_list;
    
    printf("number of allocated: %d\n", allocated_space);
    printf("max neuron id: %d number of neurons: %d\n", max_neuron_id, number_of_neurons);
    printf("Neuron State:\n");
    for (int i = 0; i < number_of_neurons; ++i) {
#ifdef CONTINUOUS_PARAM
        printf("%d:%f ", i, neuron_state[i]);
#else
        printf("%d:%i ", i, neuron_state[i]);
#endif
    }
    printf("\n");
    printf("Previous Neuron State:\n");
    for (int i = 0; i < number_of_neurons; ++i) {
#ifdef CONTINUOUS_PARAM
        printf("%d:%f ", i, previous_neuron_state[i]);
#else
        printf("%d:%i ", i, previous_neuron_state[i]);
#endif
    }
    printf("\n");
    printf("Neuron Excitation(positive) / Inhibition(negative):\n");
    for (int i = 0; i < number_of_neurons; ++i) {
#ifdef CONTINUOUS_PARAM
        printf("%d:%f ", i, neuron_excitation[i]);
#else
        printf("%d:%i ", i, neuron_excitation[i]);
#endif
    }
    printf("\n");
    printf("Map ID to DNA's Position:\n");
    for (int i = 0; i < max_neuron_id + 1; ++i) {
        printf("%d:%d ", i, mod->neuronIdToDNAIndex(i));
    }
    printf("\n");
    printf("Primer List:\n");
    for (int i = 0; primer_list[i] != -1; ++i) {
        printf("%d:%d ", i, primer_list[i]);
    }
    printf("\n");
}

void ModuleDebug::printDNA(Module *mod)
{
    auto n = mod->n;
    auto c = mod->c;
    
    printf("Printing DNA...\n");
    printf("Neurons:\n");
    printf("\n");
    int i;
    for (i = 0; n[i].id >= 0; ++i) {
        printf("id %d\n", n[i].id);
        printf("firing_rate %d\n", n[i].firing_rate);
        printf("type ");
        printNeuronType(n[i].type);
        printf("\n");
        printf("\n");
    }
    
    printf("Connections:\n");
    printf("\n");
    for (i = 0; c[i].from_neuron_id >= 0; ++i) {
        printf("%d -> %d\n", c[i].from_neuron_id, c[i].to_neuron_id);
        if (c[i].neuro_modulation < 0) {
#ifdef CONTINUOUS_PARAM
            printf("weight %f \n", c[i].weight);
#else
            printf("weight %i \n", c[i].weight);
#endif
        }
        else {
            printf("neuro modulation %d \n", c[i].neuro_modulation);
        }
        printf("\n");
    }
}

void ModuleDebug::printDNA(Module *mod, const char *filename)
{
    auto n = mod->n;
    auto c = mod->c;
    
    FILE *fp = fopen(filename,"w");
    fprintf(fp, "Printing DNA...\n");
    fprintf(fp, "Neurons:\n");
    fprintf(fp, "\n");
    
    int i;
    for (i = 0; n[i].id >= 0; ++i) {
        fprintf(fp, "id %d\n", n[i].id);
        fprintf(fp, "firing_rate %d\n", n[i].firing_rate);
        fprintf(fp, "type ");
        fprintNeuronType(fp, n[i].type);
        fprintf(fp, "\n");
        fprintf(fp, "\n");
    }
    
    printf("Connections:\n");
    printf("\n");
    
    for (i = 0; c[i].from_neuron_id >= 0; ++i) {
        fprintf(fp, "%d -> %d\n", c[i].from_neuron_id, c[i].to_neuron_id);
        if (c[i].neuro_modulation < 0) {
#ifdef CONTINUOUS_PARAM
            fprintf(fp, "weight %f \n", c[i].weight);
#else
            fprintf(fp, "weight %i \n", c[i].weight);
#endif
        }
        else {
            fprintf(fp, "neuro modulation %d \n", c[i].neuro_modulation);
        }
        fprintf(fp, "\n");
    }
    
    fclose(fp);
}

void ModuleDebug::printInternalStates(Module *mod)
{
    auto n = mod->n;
    auto neuron_excitation = mod->neuron_excitation;
    auto number_of_neurons = mod->number_of_neurons;
    auto internal_neuron_state = mod->internal_neuron_state;
    
    printf("\n");
    for (int i = 0; i < number_of_neurons; ++i) {
        printf("internal states ");
        printNeuronType(n[i].type);
#ifdef CONTINUOUS_PARAM
        printf(" %d: %f \n", n[i].id, internal_neuron_state[i]);
#else
        printf(" %d: %i \n", n[i].id, internal_neuron_state[i]);
#endif
    }
    
    printf("\n");
    for (int i = 0; i < number_of_neurons; ++i) {
        printf("excited ");
        printNeuronType(n[i].type);
#ifdef CONTINUOUS_PARAM
        printf(" %d: %f \n", n[i].id, neuron_excitation[i]);
#else
        printf(" %d: %i \n", n[i].id, neuron_excitation[i]);
#endif
    }
}

void ModuleDebug::printFiredStates(Module *mod)
{
    auto n = mod->n;
    auto is_fired = mod->is_fired;
    auto number_of_neurons = mod->number_of_neurons;
    
    printf("\n");
    printf("Fired Or Not Fired\n");
    
    for (int i = 0; i < number_of_neurons; ++i) {
        printNeuronType(n[i].type);
        printf(" %d: %d \n", n[i].id, is_fired[i]);
    }
}
