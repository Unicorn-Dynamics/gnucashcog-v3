/**
 * PatternDomainMapperWithAxisMundi.js
 * 
 * Enhanced Pattern Domain Mapper that incorporates Axis Mundi archetypes
 * and plasma mythology concepts into the pattern language framework.
 */

class PatternDomainMapperWithAxisMundi {
  constructor() {
    // Original domain ranges
    this.domainRanges = {
      towns: { start: 1, end: 94 },
      buildings: { start: 95, end: 204 },
      construction: { start: 205, end: 253 },
      regional: { start: 254, end: 286 }
    };
    
    // Axis Mundi as the transcendent pattern (719)
    this.axisMundi = 719;
    
    // OEIS A000081 sequence mapping
    this.oeisA000081 = [1, 1, 2, 4, 9, 20, 48, 115, 286, 719];
    
    // Domain activity tracking
    this.domainActivity = {
      towns: 0,
      buildings: 0,
      construction: 0,
      regional: 0
    };
    
    // Cosmic realm mapping
    this.cosmicRealms = {
      towns: {
        name: "Cosmic Geography",
        description: "Large-scale organization of cosmic space",
        archetypes: ["WorldMountain", "CosmicOcean", "SacredEnclosure", "CelestialCity"]
      },
      buildings: {
        name: "Intermediate Structures",
        description: "Connecting structures between cosmic levels",
        archetypes: ["WorldTree", "CosmicPillar", "StairwayToHeaven", "SacredTemple"]
      },
      construction: {
        name: "Elemental Components",
        description: "Basic building blocks of cosmic structure",
        archetypes: ["CosmicRope", "DivineCraftsman", "PrimordialMaterials", "SacredGeometry"]
      },
      regional: {
        name: "Transformation Processes",
        description: "Changes in cosmic structure over time",
        archetypes: ["CosmicCycle", "DivineMediator", "WorldRenewal", "CosmicHarmony"]
      }
    };
    
    // Archetype registry
    this.archetypeRegistry = this.initializeArchetypeRegistry();
    
    // Cosmic creation stages mapping to bootstrap levels
    this.cosmicCreationStages = [
      { level: 0, name: "PrimordialVoid", description: "Undifferentiated state before distinction" },
      { level: 1, name: "FirstSeparation", description: "Initial binary division of cosmos" },
      { level: 2, name: "CosmicScaffolding", description: "Basic structure of the cosmos" },
      { level: 3, name: "RealmFormation", description: "Differentiation of cosmic domains" },
      { level: 4, name: "AxisEstablishment", description: "Formation of central organizing principle" },
      { level: 5, name: "DetailedElaboration", description: "Development of complex structures" },
      { level: 6, name: "SelfReflection", description: "System becomes aware of itself" }
    ];
    
    // Axis Mundi manifestation thresholds
    this.manifestationThresholds = {
      dormant: 0.25,
      emerging: 0.5,
      active: 0.75,
      transcendent: 0.9
    };
    
    // Prime number associations
    this.primeAssociations = {
      2: { domain: "towns", concept: "FirstDistinction" },
      3: { domain: "buildings", concept: "TriangularStability" },
      5: { domain: "construction", concept: "PentagonalHarmony" },
      7: { domain: "regional", concept: "SeptenaryCompletion" },
      719: { domain: "axisMundi", concept: "TranscendentStructure" }
    };
  }
  
  /**
   * Initialize the archetype registry with detailed archetype data
   */
  initializeArchetypeRegistry() {
    return {
      // Towns domain archetypes
      "WorldMountain": {
        name: "World Mountain",
        description: "A cosmic mountain at the center of the world",
        variants: ["Mount Meru", "Mount Olympus", "Mount Sumeru"],
        patternAssociations: [1, 2, 3, 8, 15],
        visualElements: {
          primarySymbol: "mountain",
          colorAssociation: "#8a7967",
          geometricForm: "pyramid"
        }
      },
      "CosmicOcean": {
        name: "Cosmic Ocean",
        description: "Primordial waters surrounding the organized cosmos",
        variants: ["Waters of Nun", "Oceanus", "Cosmic Sea"],
        patternAssociations: [6, 25, 64],
        visualElements: {
          primarySymbol: "waves",
          colorAssociation: "#1a237e",
          geometricForm: "undulating"
        }
      },
      "SacredEnclosure": {
        name: "Sacred Enclosure",
        description: "Bounded cosmic territory with special significance",
        variants: ["Garden of Eden", "Temenos", "Sacred Grove"],
        patternAssociations: [13, 37, 60],
        visualElements: {
          primarySymbol: "circle",
          colorAssociation: "#388e3c",
          geometricForm: "enclosed"
        }
      },
      "CelestialCity": {
        name: "Celestial City",
        description: "Organized heavenly realm or divine city",
        variants: ["New Jerusalem", "City of the Gods", "Celestial Palace"],
        patternAssociations: [14, 16, 18],
        visualElements: {
          primarySymbol: "city",
          colorAssociation: "#ffd700",
          geometricForm: "gridded"
        }
      },
      
      // Buildings domain archetypes
      "WorldTree": {
        name: "World Tree",
        description: "A cosmic tree connecting the realms of the universe",
        variants: ["Yggdrasil", "Tree of Life", "Cosmic Ash"],
        patternAssociations: [95, 99, 107],
        visualElements: {
          primarySymbol: "tree",
          colorAssociation: "#3a7d44",
          geometricForm: "branching"
        }
      },
      "CosmicPillar": {
        name: "Cosmic Pillar",
        description: "A column supporting the heavens and connecting realms",
        variants: ["Atlas", "Pillar of Heaven", "Cosmic Column"],
        patternAssociations: [98, 134, 160],
        visualElements: {
          primarySymbol: "pillar",
          colorAssociation: "#b71c1c",
          geometricForm: "columnar"
        }
      },
      "StairwayToHeaven": {
        name: "Stairway to Heaven",
        description: "Graduated ascent between cosmic realms",
        variants: ["Jacob's Ladder", "Shamanic Ladder", "Rainbow Bridge"],
        patternAssociations: [112, 158, 190],
        visualElements: {
          primarySymbol: "ladder",
          colorAssociation: "#9c27b0",
          geometricForm: "stepped"
        }
      },
      "SacredTemple": {
        name: "Sacred Temple",
        description: "Microcosm of universal order",
        variants: ["Cosmic House", "Divine Dwelling", "Temple of Creation"],
        patternAssociations: [104, 129, 185],
        visualElements: {
          primarySymbol: "temple",
          colorAssociation: "#ff9800",
          geometricForm: "symmetrical"
        }
      },
      
      // Construction domain archetypes
      "CosmicRope": {
        name: "Cosmic Rope",
        description: "Binding element connecting cosmic realms",
        variants: ["World Chain", "Cosmic Cord", "Divine Thread"],
        patternAssociations: [207, 212, 221],
        visualElements: {
          primarySymbol: "rope",
          colorAssociation: "#5d4037",
          geometricForm: "twisted"
        }
      },
      "DivineCraftsman": {
        name: "Divine Craftsman",
        description: "Cosmic builder or architect",
        variants: ["Demiurge", "Divine Smith", "Creator God"],
        patternAssociations: [205, 208, 213],
        visualElements: {
          primarySymbol: "hammer",
          colorAssociation: "#607d8b",
          geometricForm: "angular"
        }
      },
      "PrimordialMaterials": {
        name: "Primordial Materials",
        description: "Cosmic building substances",
        variants: ["Prima Materia", "Cosmic Clay", "Divine Elements"],
        patternAssociations: [215, 235, 248],
        visualElements: {
          primarySymbol: "elements",
          colorAssociation: "#795548",
          geometricForm: "amorphous"
        }
      },
      "SacredGeometry": {
        name: "Sacred Geometry",
        description: "Organizing principles of cosmic structure",
        variants: ["Divine Proportion", "Cosmic Measurement", "Celestial Mathematics"],
        patternAssociations: [216, 223, 241],
        visualElements: {
          primarySymbol: "geometry",
          colorAssociation: "#03a9f4",
          geometricForm: "geometric"
        }
      },
      
      // Regional domain archetypes
      "CosmicCycle": {
        name: "Cosmic Cycle",
        description: "Recurring pattern of creation and destruction",
        variants: ["Great Year", "World Ages", "Cosmic Wheel"],
        patternAssociations: [254, 260, 267],
        visualElements: {
          primarySymbol: "cycle",
          colorAssociation: "#673ab7",
          geometricForm: "circular"
        }
      },
      "DivineMediator": {
        name: "Divine Mediator",
        description: "Being that moves between cosmic realms",
        variants: ["Cosmic Messenger", "Divine Intermediary", "Realm Walker"],
        patternAssociations: [258, 271, 280],
        visualElements: {
          primarySymbol: "winged-being",
          colorAssociation: "#2196f3",
          geometricForm: "bridging"
        }
      },
      "WorldRenewal": {
        name: "World Renewal",
        description: "Periodic cosmic regeneration",
        variants: ["Phoenix Rebirth", "Cosmic Rejuvenation", "New Creation"],
        patternAssociations: [262, 275, 283],
        visualElements: {
          primarySymbol: "phoenix",
          colorAssociation: "#ff5722",
          geometricForm: "spiraling"
        }
      },
      "CosmicHarmony": {
        name: "Cosmic Harmony",
        description: "Balanced integration of opposing forces",
        variants: ["Yin-Yang", "Cosmic Balance", "Divine Order"],
        patternAssociations: [264, 277, 285],
        visualElements: {
          primarySymbol: "balance",
          colorAssociation: "#9e9e9e",
          geometricForm: "balanced"
        }
      },
      
      // Axis Mundi archetype
      "AxisMundi": {
        name: "Axis Mundi",
        description: "Central cosmic pillar connecting all realms",
        variants: ["World Axis", "Cosmic Axis", "Columna Mundi"],
        patternAssociations: [719],
        visualElements: {
          primarySymbol: "axis",
          colorAssociation: "#e91e63",
          geometricForm: "axial"
        }
      }
    };
  }
  
  /**
   * Determine which domain a pattern belongs to
   * @param {number} patternNumber - The pattern number
   * @returns {string} The domain name
   */
  getDomainForPattern(patternNumber) {
    if (patternNumber === this.axisMundi) {
      return "axisMundi";
    }
    
    for (const [domain, range] of Object.entries(this.domainRanges)) {
      if (patternNumber >= range.start && patternNumber <= range.end) {
        return domain;
      }
    }
    
    return null;
  }
  
  /**
   * Get archetypes associated with a pattern
   * @param {number} patternNumber - The pattern number
   * @returns {Array} Array of associated archetypes
   */
  getArchetypesForPattern(patternNumber) {
    const domain = this.getDomainForPattern(patternNumber);
    if (!domain) return [];
    
    const associatedArchetypes = [];
    
    // Check all archetypes for associations with this pattern
    for (const [id, archetype] of Object.entries(this.archetypeRegistry)) {
      if (archetype.patternAssociations.includes(patternNumber)) {
        associatedArchetypes.push({
          id,
          ...archetype
        });
      }
    }
    
    // If no specific associations, return domain archetypes
    if (associatedArchetypes.length === 0 && domain !== "axisMundi") {
      const domainArchetypes = this.cosmicRealms[domain].archetypes;
      for (const archetypeId of domainArchetypes) {
        associatedArchetypes.push({
          id: archetypeId,
          ...this.archetypeRegistry[archetypeId]
        });
      }
    }
    
    return associatedArchetypes;
  }
  
  /**
   * Get the cosmic realm associated with a domain
   * @param {string} domain - The domain name
   * @returns {Object} The cosmic realm data
   */
  getCosmicRealmForDomain(domain) {
    if (domain === "axisMundi") {
      return {
        name: "Transcendent Axis",
        description: "Central axis connecting all cosmic realms",
        archetypes: ["AxisMundi"]
      };
    }
    
    return this.cosmicRealms[domain] || null;
  }
  
  /**
   * Map a pattern to a mythological archetype
   * @param {number} patternNumber - The pattern number
   * @param {string} archetypeId - The archetype ID
   */
  mapPatternToArchetype(patternNumber, archetypeId) {
    const archetype = this.archetypeRegistry[archetypeId];
    if (!archetype) return false;
    
    // Add pattern to archetype's associations if not already present
    if (!archetype.patternAssociations.includes(patternNumber)) {
      archetype.patternAssociations.push(patternNumber);
    }
    
    return true;
  }
  
  /**
   * Generate a cosmic narrative for pattern activation
   * @param {number} patternNumber - The pattern number
   * @param {string} activationType - The type of activation
   * @returns {string} The generated narrative
   */
  generateCosmicNarrative(patternNumber, activationType) {
    const domain = this.getDomainForPattern(patternNumber);
    if (!domain) return "";
    
    const archetypes = this.getArchetypesForPattern(patternNumber);
    if (archetypes.length === 0) return "";
    
    const archetype = archetypes[0]; // Use first archetype for narrative
    const realm = this.getCosmicRealmForDomain(domain);
    
    // Narrative templates
    const narrativeTemplates = {
      activation: `The ${archetype.name} emerges within the ${realm.name}, bringing forth new patterns of order.`,
      transformation: `The ${archetype.name} transforms, reshaping the structure of the ${realm.name}.`,
      integration: `The ${archetype.name} establishes connections with other realms, strengthening the cosmic fabric.`,
      manifestation: `The ${archetype.name} fully manifests, revealing its true nature and power.`
    };
    
    return narrativeTemplates[activationType] || 
      `The ${archetype.name} responds to your interaction within the ${realm.name}.`;
  }
  
  /**
   * Update domain activity based on pattern activation
   * @param {number} patternNumber - The pattern number
   * @param {number} activityLevel - The activity level (0-1)
   */
  updateDomainActivity(patternNumber, activityLevel) {
    const domain = this.getDomainForPattern(patternNumber);
    if (!domain || domain === "axisMundi") return;
    
    this.domainActivity[domain] = Math.max(
      this.domainActivity[domain],
      activityLevel
    );
  }
  
  /**
   * Calculate the overall Axis Mundi manifestation level
   * @returns {number} Manifestation level (0-1)
   */
  calculateAxisMundiManifestation() {
    // Calculate average domain activity
    const domainValues = Object.values(this.domainActivity);
    const avgActivity = domainValues.reduce((sum, val) => sum + val, 0) / domainValues.length;
    
    // Calculate balance factor (higher when domains are balanced)
    const minActivity = Math.min(...domainValues);
    const balanceFactor = minActivity / (avgActivity || 1);
    
    // Combined manifestation level
    return avgActivity * balanceFactor;
  }
  
  /**
   * Get the manifestation state of the Axis Mundi
   * @returns {string} Manifestation state
   */
  getAxisMundiManifestationState() {
    const level = this.calculateAxisMundiManifestation();
    
    if (level >= this.manifestationThresholds.transcendent) {
      return "transcendent";
    } else if (level >= this.manifestationThresholds.active) {
      return "active";
    } else if (level >= this.manifestationThresholds.emerging) {
      return "emerging";
    } else {
      return "dormant";
    }
  }
  
  /**
   * Get the cosmic creation stage for a bootstrap level
   * @param {number} level - The bootstrap level
   * @returns {Object} The cosmic creation stage
   */
  getCreationStageForBootstrapLevel(level) {
    return this.cosmicCreationStages[level] || this.cosmicCreationStages[0];
  }
  
  /**
   * Generate a transformation sequence for pattern integration
   * @param {number} patternNumber - The pattern number
   * @returns {Array} Transformation sequence
   */
  generateCosmicTransformation(patternNumber) {
    const domain = this.getDomainForPattern(patternNumber);
    if (!domain) return [];
    
    const archetypes = this.getArchetypesForPattern(patternNumber);
    if (archetypes.length === 0) return [];
    
    const archetype = archetypes[0]; // Use first archetype for transformation
    
    // Domain-specific transformation sequences
    const transformationSequences = {
      towns: ["Separation", "Orientation", "Boundary", "Center"],
      buildings: ["Foundation", "Connection", "Ascension", "Integration"],
      construction: ["Material", "Form", "Structure", "Function"],
      regional: ["Cycle", "Mediation", "Renewal", "Harmony"],
      axisMundi: ["Emergence", "Connection", "Stabilization", "Transcendence"]
    };
    
    return transformationSequences[domain].map(stage => ({
      stage,
      archetype: archetype.name,
      description: `${stage} of ${archetype.name}`,
      visualEffect: `${stage.toLowerCase()}-${archetype.visualElements.geometricForm}`
    }));
  }
  
  /**
   * Arrange patterns by cosmic hierarchy for ritual
   * @param {Object} selectedPatterns - Object mapping domains to pattern numbers
   * @returns {Array} Optimal arrangement of patterns
   */
  arrangePatternsByCosmicHierarchy(selectedPatterns) {
    const arrangement = [];
    
    // Order domains by cosmic hierarchy (bottom to top)
    const hierarchicalOrder = ["construction", "towns", "buildings", "regional"];
    
    // Add patterns in hierarchical order
    for (const domain of hierarchicalOrder) {
      if (selectedPatterns[domain]) {
        arrangement.push(selectedPatterns[domain]);
      }
    }
    
    return arrangement;
  }
  
  /**
   * Establish Axis Mundi connection when sufficient integration is achieved
   * @returns {boolean} Whether connection was established
   */
  establishAxisMundiConnection() {
    const manifestationLevel = this.calculateAxisMundiManifestation();
    
    // Connection requires transcendent manifestation level
    if (manifestationLevel >= this.manifestationThresholds.transcendent) {
      // Activate the Axis Mundi pattern
      return true;
    }
    
    return false;
  }
  
  /**
   * Get prime number association for a domain
   * @param {string} domain - The domain name
   * @returns {Object} Prime association data
   */
  getPrimeAssociationForDomain(domain) {
    for (const [prime, association] of Object.entries(this.primeAssociations)) {
      if (association.domain === domain) {
        return {
          prime: parseInt(prime),
          concept: association.concept
        };
      }
    }
    
    return null;
  }
}

// Export the class
export default PatternDomainMapperWithAxisMundi;
