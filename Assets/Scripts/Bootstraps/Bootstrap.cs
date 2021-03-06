﻿using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Unity.Entities;
using Unity.Mathematics;
using Zoxel.Voxels;

namespace Zoxel
{
    // ideas
    // A log that shows world generation happening when making the game
    //      Procedurally create factions at start of game generation - save game data
    //      also create a world biome map
    //      on top of this create town locations
    //      on top of this create rivers and mountains
    // genetics that creates character models
    // skeleton system
    // game mode should spawn other systems depending on game type
    // dynamic animations - like swinging arms and legs and IK for feet positions

    /// <summary>
    /// Boot Code for Zoxel
    /// </summary>
    [RequireComponent(typeof(AudioManager), typeof(ParticlesManager))]
    public class Bootstrap : MonoBehaviour
    {
        public static Bootstrap instance;
        public static bool isAudio = true;
        public static bool isRenderChunks = true;
        public static bool isParticles = true;
        public static bool isStreamChunks = true;
        [Header("Settings")]
        public bool isCustomRenderSystem = false;
        public bool isMutateVoxes = false;
        public bool isAnimateRenders = false;
        public bool isMonsters = false;
        [Header("Debug Settings")]
        public bool isDebugWeightColours;
        public bool isAddAnimations = true;
        public bool isUseModels = true;
        public bool DebugColliders = true;
        // debugs
        public static bool DebugChunks = false;
        public static bool DebugMeshWeights = false;

        public int renderDistance = 3;
        public int loadDistance = 4;
        public int mapResolution = 4;
        
        public bool isBiomeMaps;
        [Header("Data")]
        public GameDatam data;
        public Entity game;
        private SystemsManager sys;
        public GameObject canvas;
        private Entity gameText;
        private Entity startText;

        public Material voxelMaterial;

        #region Booting

        public static Material GetVoxelMaterial()
        {
            // could be new Material if need fading!
            return Bootstrap.instance.voxelMaterial;
        }

        public EntityManager EntityManager
        {
            get { 
                if (sys == null) {
                    return null;
                }
                return sys.space.EntityManager; 
            }
        }

        public void Awake()
        {
            instance = this;
            Debug.Log("Awakening the Booty.");
            #if UNITY_EDITOR
            LoadGameData();
            #endif
        }

        [ContextMenu("Load Game Data")]
        private void LoadGameData()
        {
            #if UNITY_EDITOR
            data.characters = LoadFolder<CharacterDatam>(Application.dataPath + "/Data/Characters/", "Characters");
            data.quests = LoadFolder<QuestDatam>(Application.dataPath + "/Data/Quests/", "Quests");
            data.dialogues = LoadFolder<DialogueDatam>(Application.dataPath + "/Data/Dialogues/", "Dialogues");
            data.stats = LoadFolder<StatDatam>(Application.dataPath + "/Data/Stats/", "Stats");
            data.items = LoadFolder<ItemDatam>(Application.dataPath + "/Data/Items/", "Items");
            data.items.AddRange(LoadFolder<ItemDatam>(Application.dataPath + "/Data/Voxes/", "Voxes"));
            data.skills = LoadFolder<SkillDatam>(Application.dataPath + "/Data/Skills/", "Skills");
            data.voxels = LoadFolder<VoxelDatam>(Application.dataPath + "/Data/Voxels/", "Voxels");
            data.models = LoadFolder<VoxDatam>(Application.dataPath + "/Data/Voxes/", "Voxes");
            data.bullets = LoadFolder<BulletDatam>(Application.dataPath + "/Data/Bullets/", "Bullets");
            #endif
        }

        public static List<T> LoadFolder<T>(string folderPath, string folderName) where T : ScriptableObject
        {
            List<T> datams = new List<T>();
            #if UNITY_EDITOR
            var files = System.IO.Directory.GetFiles(folderPath);
            foreach (var filepath in files)
            {
                if (filepath.Contains(".meta") == false)
                {
                    string newPath = filepath.Substring(filepath.IndexOf("Assets/Data/" + folderName + "/"));
                    var statA = UnityEditor.AssetDatabase.LoadAssetAtPath<T>(newPath);
                    if (statA != null)
                    {
                        datams.Add(statA);
                    }
                }
            }
            var folders = System.IO.Directory.GetDirectories(folderPath);
            foreach (var folder in folders)
            {
                datams.AddRange(LoadFolder<T>(folder, folderName));
            }
            #endif
            return datams;
        }

        public bool Initialize(string systems)
        {
            Debug.Log("Initializing the Booty.");
            return true;
        }

        public Color titleColor = Color.cyan;
        public Color subTitleColor = Color.red;

        public void Start()
        {
            sys = new SystemsManager(data, "ZoxelGame");
            sys.voxelSystemGroup.voxelPreviewSystem.Test();
            gameText = UIUtilities.SpawnText(EntityManager, new Entity(), "Zoxel", 
                new float3(0, 1f, 0.3f), titleColor, 0.07f);
            startText = UIUtilities.SpawnText(EntityManager, new Entity(), "Press Any Key to Start",
                new float3(0, 0.85f, 0.3f), subTitleColor, 0.022f);
            game = sys.gameSystemGroup.gameStartSystem.CreateGame(data);
            sys.gameSystemGroup.gameStartSystem.OnStarted += () =>
            {
                EntityManager.DestroyEntity(gameText);
                EntityManager.DestroyEntity(startText);
            };
            sys.uiSystemGroup.menuSpawnSystem.game = game;
        }


        public void LateUpdate()
        {
            if (sys != null && sys.cameraSystemGroup != null)
            {
                var entityManager = EntityManager;
                var portalSystem = sys.space.GetOrCreateSystem<PortalSystem>();
                //sys.portalSystemGroup.portalSystem.Update();
                sys.cameraSystemGroup.cameraSystem.SynchCameras();
                portalSystem.ManualUpdate();
            }
        }

        public static int GetRenderDistance()
        {
            if (instance)
            {
                return instance.renderDistance;
            }
            return 3;
        }

        public static int GetLoadDistance()
        {
            if (instance)
            {
                return instance.loadDistance;
            }
            return 4;
        }

        public static int GenerateUniqueID()
        {
            return System.Guid.NewGuid().GetHashCode();
        }
        #endregion


        #region EditorFunctions
        public SystemsManager GetSystems()
        {
            return sys;
        }

#if UNITY_EDITOR
        [ContextMenu("Open Save Folder")]
        public void OpenSaveFolder()
        {
            //Debug.LogError("Opening Save Folder: " + SaveManager.GetSavePath());
            string argument = "/C start" + " " + SaveSystem.GetSavePath();
            Debug.Log("Save Folder: " + argument);
            //var process = System.Diagnostics.Process.Start("cmd.exe", argument);
    #if UNITY_EDITOR_WIN
            System.Diagnostics.Process process = new System.Diagnostics.Process();
            System.Diagnostics.ProcessStartInfo startInfo = new System.Diagnostics.ProcessStartInfo();
            startInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden;
            startInfo.FileName = "cmd.exe";
            startInfo.Arguments = argument;// "/C copy /b Image1.jpg + Archive.rar Image2.jpg";
            process.StartInfo = startInfo;
            process.Start();
    #endif
        }
#endif

        [Header("Test Functions")]
        public QuestDatam quest;
        public ItemDatam item;
        public List<SkillDatam> skills;
        public MapDatam map;
        public StatDatam stat;
        public int amountToSpawn;
        public int clanToSpawn;
        public float3 spawnPosition = new float3(8, 42, 8);
        public CharacterDatam character;
        public int GameID
        {
            get { return EntityManager.GetComponentData<Game>(game).id; }
        }
        public Entity World
        {
            get { return EntityManager.GetComponentData<Game>(game).map; }
        }

        [ContextMenu("GiveStatToPlayers")]
        public void GiveStatToPlayers()
        {
            foreach (var player in sys.playerSystemGroup.playerSpawnSystem.controllers.Values)
            {
                Stats stats = EntityManager.GetComponentData<Stats>(player);
                int didAdd = stats.AddStat(stat);
                if (didAdd != -1)
                {
                    EntityManager.SetComponentData(player, stats);
                }
            }
        }

        [ContextMenu("GiveQuestToPlayers")]
        public void GiveQuestToPlayers()
        {
            if (quest != null)
            {
                foreach (var player in sys.playerSystemGroup.playerSpawnSystem.controllers.Values)
                {
                    QuestLog questlog = EntityManager.GetComponentData<QuestLog>(player);
                    System.Collections.Generic.List<QuestDatam> quests = new System.Collections.Generic.List<QuestDatam>();
                    quests.Add(quest);
                    questlog.Initialize(quests);
                    EntityManager.SetComponentData(player, questlog);
                }
            }
        }

        [ContextMenu("GiveItemToPlayers")]
        public void GiveItemToPlayers()
        {
            if (quest != null)
            {
                foreach (var player in sys.playerSystemGroup.playerSpawnSystem.controllers.Values)
                {
                    Inventory inventory = EntityManager.GetComponentData<Inventory>(player);
                    System.Collections.Generic.List<ItemDatam> items = new System.Collections.Generic.List<ItemDatam>();
                    items.Add(item);
                    inventory.InitializeItems(9, items);
                    EntityManager.SetComponentData(player, inventory);
                }
            }
        }

        [ContextMenu("GiveSkillsToPlayers")]
        public void GiveSkillsToPlayers()
        {
            foreach (var player in sys.playerSystemGroup.playerSpawnSystem.controllers.Values)
            {
                Skills skillsC = EntityManager.GetComponentData<Skills>(player);
                skillsC.Initialize(skills);
                EntityManager.SetComponentData(player, skillsC);
                sys.skillSystemGroup.skillsSystem.InitializeSkills(player, skillsC);
            }
        }

        [ContextMenu("GiveExperienceToPlayers")]
        public void GiveExperienceToPlayers()
        {
            foreach (var player in sys.playerSystemGroup.playerSpawnSystem.controllers.Values)
            {
                foreach (var defender in sys.characterSystemGroup.characterSpawnSystem.characters.Values)
                {
                    if (player.Index != defender.Index)
                    {
                        DamageSystem.AddDamage(EntityManager, player, defender, 0, UnityEngine.Random.Range(1, 12));
                        //break;
                    }
                }
            }
        }

        /// <summary>
        /// loading new map should reposition players after if their characters already exist
        /// </summary>
        [ContextMenu("LoadMap")]
        public void LoadMap()
        {

        }

        [ContextMenu("SpawnCharacters")]
        public void SpawnCharacters()
        {
            CharacterSpawnSystem.SpawnNPCs(EntityManager,
                World, character.Value.id, clanToSpawn, spawnPosition, amountToSpawn);
        }

        public IEnumerator RunSpawnCharactersTest()
        {
            for (int i = 0; i < 10; i++)
            {
                yield return new WaitForSeconds(20);
                SpawnCharacters();
            }
        }
        #endregion

    }

}