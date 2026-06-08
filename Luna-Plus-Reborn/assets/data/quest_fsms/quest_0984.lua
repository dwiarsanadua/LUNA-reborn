--[[
  Quest 984: [Dungeon Quest] Secrets of the Temple(Intermediate)
  Level Required: 90
  NPC Start: 64, NPC Complete: 64
  Rewards: EXP=4122943, Gold=61188
]]

local fsm = require('fsm_engine')

local quest_984 = fsm:new({
    id = 984,
    name = "[Dungeon Quest] Secrets of the Temple(Intermediate)",
    level_required = 90,
    npc_start = 64,
    npc_complete = 64,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 984: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 984: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 984: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 667,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 984: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 668,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 984: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 669,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 984: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 670,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 984: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 984: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 984: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 984: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 984: Rewards given")
                self:give_rewards({
                    exp = 4122943,
                    gold = 61188,
                    items = {
                        { item_id = 21000010, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_984