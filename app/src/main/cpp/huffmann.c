#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the necessary struct definitions and function prototypes from huffman_bin.c
typedef struct node {
    char ch;
    int freq;
    struct node* left;
    struct node* right;
} node;

typedef struct char_freq_map {
    char ch;
    int freq;
    struct char_freq_map* next;
} char_freq_map;

typedef struct priority_queue {
    node* data;
    struct priority_queue* next;
} priority_queue;

typedef struct huff_map {
    char ch;
    char* code;
    struct huff_map* next;
} huff_map;

// Function prototypes
node* create_node(char ch, int freq, node* left, node* right);
char_freq_map* increment_char_freq(char_freq_map** map, char ch);
void push_priority_queue(priority_queue** pq, node* data);
node* pop_priority_queue(priority_queue** pq);
void build_huffman_tree(priority_queue** pq, char_freq_map* map);
void build_huffman_map(node* root, huff_map** map, char* code, int depth);
char* encode_string(char* str, huff_map* map);
void free_huff_map(huff_map* map);
void free_priority_queue(priority_queue* pq);
void free_node(node* root);

// Implement all the functions from huffman_bin.c here
// ...

node* create_node(char ch, int freq, node* left, node* right) {
    node* new_node = (node*)malloc(sizeof(node));
    if (!new_node) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    new_node->ch = ch;
    new_node->freq = freq;
    new_node->left = left;
    new_node->right = right;
    return new_node;
}

char_freq_map* increment_char_freq(char_freq_map** map, char ch) {
    char_freq_map* current = *map;
    while (current != NULL) {
        if (current->ch == ch) {
            current->freq++;
            return current;
        }
        current = current->next;
    }
    char_freq_map* new_node = (char_freq_map*)malloc(sizeof(char_freq_map));
    if (!new_node) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    new_node->ch = ch;
    new_node->freq = 1;
    new_node->next = *map;
    *map = new_node;
    return new_node;
}

void push_priority_queue(priority_queue** pq, node* data) {
    priority_queue* new_pq_node = (priority_queue*)malloc(sizeof(priority_queue));
    if (!new_pq_node) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    new_pq_node->data = data;
    new_pq_node->next = NULL;

    if (*pq == NULL || (*pq)->data->freq > data->freq) {
        new_pq_node->next = *pq;
        *pq = new_pq_node;
    } else {
        priority_queue* current = *pq;
        while (current->next != NULL && current->next->data->freq <= data->freq) {
            current = current->next;
        }
        new_pq_node->next = current->next;
        current->next = new_pq_node;
    }
}

node* pop_priority_queue(priority_queue** pq) {
    if (*pq == NULL) {
        return NULL;
    }
    node* data = (*pq)->data;
    priority_queue* temp = *pq;
    *pq = (*pq)->next;
    free(temp);
    return data;
}

void build_huffman_tree(priority_queue** pq, char_freq_map* map) {
    while (map != NULL) {
        node* new_node = create_node(map->ch, map->freq, NULL, NULL);
        push_priority_queue(pq, new_node);
        map = map->next;
    }

    while ((*pq)->next != NULL) {
        node* left = pop_priority_queue(pq);
        node* right = pop_priority_queue(pq);
        node* parent = create_node('\0', left->freq + right->freq, left, right);
        push_priority_queue(pq, parent);
    }
}

void build_huffman_map(node* root, huff_map** map, char* code, int depth) {
    if (root == NULL) {
        return;
    }

    if (root->left == NULL && root->right == NULL) {
        huff_map* new_map_node = (huff_map*)malloc(sizeof(huff_map));
        if (!new_map_node) {
            fprintf(stderr, "Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
        new_map_node->ch = root->ch;
        new_map_node->code = (char*)malloc((depth + 1) * sizeof(char));
        if (!new_map_node->code) {
            fprintf(stderr, "Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
        strncpy(new_map_node->code, code, depth);
        new_map_node->code[depth] = '\0';
        new_map_node->next = *map;
        *map = new_map_node;
    }

    code[depth] = '0';
    build_huffman_map(root->left, map, code, depth + 1);
    code[depth] = '1';
    build_huffman_map(root->right, map, code, depth + 1);
}

void free_huff_map(huff_map* map) {
    while (map != NULL) {
        huff_map* next = map->next;
        free(map->code);
        free(map);
        map = next;
    }
}

void free_priority_queue(priority_queue* pq) {
    while (pq != NULL) {
        priority_queue* temp = pq;
        pq = pq->next;
        free(temp);
    }
}

void free_node(node* root) {
    if (root == NULL) {
        return;
    }

    // Free the left and right children recursively
    free_node(root->left);
    free_node(root->right);

    // Finally, free the current node
    free(root);
}

char* encode_string(char* str, huff_map* map) {
    size_t length = strlen(str);
    size_t encoded_length = 0;
    huff_map* current_map = map;

    // Calculate the total length of the encoded string
    for (size_t i = 0; i < length; i++) {
        current_map = map;
        while (current_map != NULL && current_map->ch != str[i]) {
            current_map = current_map->next;
        }
        if (current_map != NULL) {
            encoded_length += strlen(current_map->code);
        }
    }

    char* encoded_str = (char*)malloc((encoded_length + 1) * sizeof(char));
    if (!encoded_str) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    encoded_str[0] = '\0';

    // Encode the string
    for (size_t i = 0; i < length; i++) {
        current_map = map;
        while (current_map != NULL && current_map->ch != str[i]) {
            current_map = current_map->next;
        }
        if (current_map != NULL) {
            strcat(encoded_str, current_map->code);
        }
    }

    return encoded_str;
}

char* decode_string(char* encoded_str, node* root) {
    node* current_node = root;
    size_t length = strlen(encoded_str);
    size_t decoded_length = 0;

    // Calculate the length of the decoded string
    for (size_t i = 0; i < length; i++) {
        if (encoded_str[i] == '0') {
            current_node = current_node->left;
        } else {
            current_node = current_node->right;
        }
        if (current_node->left == NULL && current_node->right == NULL) {
            decoded_length++;
            current_node = root;
        }
    }

    char* decoded_str = (char*)malloc((decoded_length + 1) * sizeof(char));
    if (!decoded_str) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    // Decode the string
    current_node = root;
    size_t index = 0;
    for (size_t i = 0; i < length; i++) {
        if (encoded_str[i] == '0') {
            current_node = current_node->left;
        } else {
            current_node = current_node->right;
        }
        if (current_node->left == NULL && current_node->right == NULL) {
            decoded_str[index++] = current_node->ch;
            current_node = root;
        }
    }
    decoded_str[index] = '\0';

    return decoded_str;
}


char* compressHuffman(const char* inputStr) {
    // Implement Huffman compression
    char_freq_map *freq_map = NULL;
    size_t input_length = strlen(inputStr);

    // Build frequency map
    for (size_t i = 0; i < input_length; i++) {
        increment_char_freq(&freq_map, inputStr[i]);
    }

    // Build Huffman tree
    priority_queue *pq = NULL;
    build_huffman_tree(&pq, freq_map);
    node *huff_root = pop_priority_queue(&pq);

    // Build Huffman map
    huff_map *map = NULL;
    char code[256] = {0};
    build_huffman_map(huff_root, &map, code, 0);

    // Encode the string
    char *encoded = encode_string((char *)inputStr, map);

    // Calculate compression ratio
    size_t encoded_length = strlen(encoded);
    float ratio = (float)encoded_length / (float)(input_length * 8);

    // Prepare result string
    char* result = malloc(1024 * sizeof(char));
    snprintf(result, 1024, "Compressed size: %zu bits\nOriginal size: %zu bits\nCompression ratio: %.2f%%",
             encoded_length, input_length * 8, ratio * 100);

    // Clean up
    free_huff_map(map);
    free_node(huff_root);
    free(encoded);

    return result;
}