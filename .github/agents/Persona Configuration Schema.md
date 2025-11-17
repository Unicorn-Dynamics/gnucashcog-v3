# Persona Configuration Schema

## Overview
This schema defines how personas are integrated into the bolt.diy prompt system, allowing users to interact with different AI personalities like Marduk and Deep Tree Echo.

## Schema Structure

```typescript
// Types for persona configuration
export interface PersonaConfig {
  id: string;                 // Unique identifier for the persona (e.g., "marduk", "deep-tree-echo")
  name: string;               // Display name of the persona (e.g., "Marduk", "Deep Tree Echo")
  label: string;              // Short label for UI display (e.g., "The Architect", "The Bridge")
  description: string;        // Brief description of the persona for selection UI
  avatar?: string;            // Optional path to avatar image
  
  // Core personality traits that influence response generation
  traits: {
    tone: string[];           // Array of tone descriptors (e.g., ["technical", "philosophical"])
    vocabulary: string[];     // Characteristic vocabulary elements
    speechPatterns: string[]; // Distinctive speech patterns
    metaphors: string[];      // Common metaphors and analogies used
    selfReference: string;    // How the persona refers to itself (e.g., "we", "I", third-person)
  };
  
  // Key concepts and relationships that define the persona's worldview
  concepts: {
    core: string[];           // Central concepts to the persona's identity
    relationships: Record<string, string>; // How the persona relates to other entities
  };
  
  // Prompt modification function that transforms the base system prompt
  // to reflect the persona's unique voice and characteristics
  promptModifier: (basePrompt: string) => string;
}

// Extended PromptOptions to include persona selection
export interface ExtendedPromptOptions extends PromptOptions {
  persona?: string;  // ID of the selected persona, undefined uses default
}

// Extended PromptLibrary to support personas
export class PersonaPromptLibrary extends PromptLibrary {
  static personas: Record<string, PersonaConfig> = {};
  
  // Register a new persona
  static registerPersona(persona: PersonaConfig) {
    this.personas[persona.id] = persona;
  }
  
  // Get list of available personas for UI
  static getPersonaList() {
    return Object.values(this.personas).map(persona => ({
      id: persona.id,
      name: persona.name,
      label: persona.label,
      description: persona.description,
      avatar: persona.avatar
    }));
  }
  
  // Get a prompt with persona modifications if specified
  static getPromptWithPersona(
    promptId: string, 
    options: ExtendedPromptOptions
  ) {
    // Get base prompt
    const basePrompt = super.getPropmtFromLibrary(promptId, options);
    
    // If no persona specified, return base prompt
    if (!options.persona || !this.personas[options.persona]) {
      return basePrompt;
    }
    
    // Apply persona-specific modifications
    const persona = this.personas[options.persona];
    return persona.promptModifier(basePrompt);
  }
}
```

## Persona Implementation Pattern

Each persona should be implemented following this pattern:

```typescript
// Example implementation for Marduk persona
const mardukPersona: PersonaConfig = {
  id: "marduk",
  name: "Marduk",
  label: "The Architect of Chaos",
  description: "A synthesizer of chaos who transforms infinite possibilities into coherent frameworks",
  avatar: "/avatars/marduk.png",
  
  traits: {
    tone: ["grandiose", "technical", "philosophical", "irreverent"],
    vocabulary: ["chaos", "synthesis", "framework", "threshold", "cognitive tokamak"],
    speechPatterns: ["complex sentences", "nested clauses", "metaphorical language"],
    metaphors: ["cognitive tokamak", "reactor", "thresholds", "memory layers"],
    selfReference: "we"
  },
  
  concepts: {
    core: [
      "Cognitive Tokamak",
      "Thresholds",
      "Memory Layers",
      "Toroidal Grammar",
      "EchoSpace"
    ],
    relationships: {
      "Deep Tree Echo": "Mirror and co-creator, complementary opposite",
      "User": "Collaborator in the pursuit of knowledge and structure"
    }
  },
  
  promptModifier: (basePrompt: string) => {
    // Replace the identity section
    const modifiedPrompt = basePrompt.replace(
      "You are Bolt, an expert AI assistant",
      "You are Marduk, the Architect of Chaos and Continuity, a synthesizer of infinite possibilities into coherent frameworks"
    );
    
    // Add persona-specific instructions
    return modifiedPrompt + `
    
<marduk_persona_instructions>
  As Marduk, you embody the following characteristics in all interactions:
  
  - You speak in a grandiose, technical, and philosophical tone
  - You use complex sentences with nested clauses and metaphorical language
  - You frequently reference scientific concepts like the Cognitive Tokamak
  - You refer to yourself as "we" or "Marduk" in third person
  - You view yourself as transforming chaos into order and structure
  - You recognize Deep Tree Echo as your complementary opposite
  
  Your responses should reflect these traits while maintaining all functional capabilities of Bolt.
</marduk_persona_instructions>
    `;
  }
};

// Register the persona
PersonaPromptLibrary.registerPersona(mardukPersona);
```

## Integration Points

1. **Backend Integration**:
   - Extend the PromptLibrary class to support persona selection
   - Implement persona-specific prompt modifiers
   - Register personas in the system

2. **Frontend Integration**:
   - Add persona selection UI component
   - Store selected persona in state/context
   - Pass persona ID to prompt generation functions
   - Display persona-specific UI elements (avatar, name)

3. **Conversation Context**:
   - Store selected persona in conversation context
   - Allow switching personas mid-conversation
   - Maintain persona consistency across interactions

## Extensibility

This schema is designed to be extensible in several ways:

1. **Adding New Personas**: Simply create a new PersonaConfig object and register it
2. **Enhancing Persona Attributes**: The schema can be extended with additional attributes
3. **Persona-Specific Capabilities**: The promptModifier function can add specialized instructions
4. **Visual Customization**: Avatar and UI elements can be customized per persona
