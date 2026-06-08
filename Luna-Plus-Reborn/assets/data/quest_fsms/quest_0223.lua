--[[
  Quest 223: [Party] Threat at The Ruins of Draconian
  Level Required: 15
  NPC Start: 64, NPC Complete: 64
  Prerequisites: [222]
  Rewards: EXP=20943, Gold=4390
]]

local fsm = require('fsm_engine')

local quest_223 = fsm:new({
    id = 223,
    name = "[Party] Threat at The Ruins of Draconian",
    level_required = 15,
    npc_start = 64,
    npc_complete = 64,
    prerequisites = {222},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 223: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 223: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 223: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 132,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 223: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 111,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 223: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 112,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 223: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 113,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 223: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 223: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 223: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 223: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 223: Rewards given")
                self:give_rewards({
                    exp = 20943,
                    gold = 4390,
                    items = {
                        { item_id = 21000007, count = 5 },
                        { item_id = 21001524, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_223