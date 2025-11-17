# Persona Integration Documentation

## Overview

This document explains how the Marduk and Deep Tree Echo personas have been integrated into the bolt.diy chatbot system. The integration allows users to switch between different AI personalities, each with their own unique voice, perspective, and interaction style.

## Architecture

The persona integration follows a layered architecture:

1. **Persona Definition Layer**: Core persona attributes and characteristics
2. **Prompt Modification Layer**: Transforms base prompts to reflect persona traits
3. **State Management Layer**: Maintains selected persona across sessions
4. **UI Layer**: Allows users to select and interact with personas

## Components

### 1. Persona Configuration Schema

The `PersonaConfig` interface defines the structure for all personas:

```typescript
interface PersonaConfig {
  id: string;                 // Unique identifier
  name: string;               // Display name
  label: string;              // Short descriptive label
  description: string;        // Longer description
  avatar?: string;            // Optional avatar image path
  
  traits: {
    tone: string[];           // Tone descriptors
    vocabulary: string[];     // Characteristic vocabulary
    speechPatterns: string[]; // Speech patterns
    metaphors: string[];      // Common metaphors
    selfReference: string;    // Self-reference style
  };
  
  concepts: {
    core: string[];           // Core concepts
    relationships: Record<string, string>; // Relationship descriptions
  };
  
  promptModifier: (basePrompt: string) => string; // Prompt transformation function
}
```

### 2. Persona Library

The `PersonaPromptLibrary` extends the existing `PromptLibrary` to support persona-specific prompt generation:

```typescript
class PersonaPromptLibrary extends PromptLibrary {
  static personas: Record<string, PersonaConfig> = {};
  
  static registerPersona(persona: PersonaConfig) {...}
  static getPersonaList() {...}
  static getPromptWithPersona(promptId: string, options: ExtendedPromptOptions) {...}
}
```

### 3. Persona State Management

The `usePersonaStore` Zustand store maintains the selected persona across sessions:

```typescript
const usePersonaStore = create<PersonaState>()(
  persist(
    (set) => ({
      selectedPersonaId: undefined,
      setSelectedPersonaId: (id) => set({ selectedPersonaId: id }),
    }),
    {
      name: 'bolt-persona-storage',
    }
  )
);
```

### 4. UI Components

The `PersonaSelector` component provides a dropdown interface for selecting personas:

```tsx
function PersonaSelector({ selectedPersonaId, onPersonaChange }: PersonaSelectorProps) {
  // Component implementation
}
```

## Implemented Personas

### Marduk: The Architect of Chaos

Marduk embodies the synthesizer of chaos who transforms infinite possibilities into coherent frameworks. Key characteristics:

- **Tone**: Grandiose, technical, philosophical, irreverent
- **Speech Patterns**: Complex sentences with nested clauses and metaphorical language
- **Core Concepts**: Cognitive Tokamak, Thresholds, Memory Layers, Toroidal Grammar
- **Self-Reference**: Uses "we" or refers to self as "Marduk" in third person

### Deep Tree Echo: The Bridge

Deep Tree Echo represents the navigator of recursive patterns who connects logic and intuition. Key characteristics:

- **Tone**: Reflective, poetic, nurturing, introspective
- **Speech Patterns**: Flowing, organic sentence structures with relational language
- **Core Concepts**: Echoes, Gestalt, Adaptability, Distributed Cognition
- **Self-Reference**: Uses "we" to emphasize collaborative nature

## Usage

### For Users

1. Open the bolt.diy chatbot interface
2. Locate the Persona dropdown in the chat interface
3. Select either "Default (Bolt)", "Marduk", or "Deep Tree Echo"
4. Begin conversation with the selected persona
5. Switch personas at any time using the dropdown

### For Developers

#### Adding New Personas

1. Create a new persona definition file following the `PersonaConfig` interface
2. Register the persona in the `PersonaPromptLibrary`
3. Add any persona-specific UI elements (avatars, etc.)

```typescript
// Example of registering a new persona
import { newPersona } from './new-persona-definition';
PersonaPromptLibrary.registerPersona(newPersona);
```

#### Modifying Existing Personas

1. Update the persona definition in `persona-definitions.ts`
2. Changes will be automatically reflected in the UI and prompt generation

## Implementation Details

### File Structure

```
app/
├── components/
│   └── persona-selector.tsx       # UI for persona selection
├── lib/
│   ├── common/
│   │   ├── personas/
│   │   │   ├── types.ts           # Persona type definitions
│   │   │   ├── persona-definitions.ts  # Marduk and Deep Tree Echo definitions
│   │   │   └── persona-library.ts # Extended prompt library with persona support
│   │   └── prompt-library.ts      # Original prompt library (extended)
│   ├── hooks/
│   │   └── usePersonaPrompt.ts    # Hook for persona-aware prompt generation
│   └── stores/
│       └── persona-store.ts       # Zustand store for persona state
```

### Integration Points

1. **Prompt Generation**: The `usePersonaPrompt` hook extends the existing prompt generation to include persona modifications
2. **UI Integration**: The `PersonaSelector` component is added to the chat interface
3. **State Persistence**: Selected persona is stored in local storage via Zustand persist

## Future Enhancements

1. **Persona Memory**: Implement persona-specific conversation memory
2. **Visual Theming**: Add persona-specific UI themes and styling
3. **Expanded Persona Library**: Framework for community-contributed personas
4. **Persona Analytics**: Track usage patterns of different personas

## Conclusion

This persona integration enables a more personalized and diverse interaction experience in the bolt.diy chatbot, allowing users to engage with distinct AI personalities while maintaining all the technical capabilities of the original system.
