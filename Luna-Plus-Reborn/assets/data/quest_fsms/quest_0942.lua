--[[
  Quest 942: [Party] Soothing Silence
  Level Required: 105
  NPC Start: 542, NPC Complete: 542
  Rewards: EXP=12417633, Gold=101632
]]

local fsm = require('fsm_engine')

local quest_942 = fsm:new({
    id = 942,
    name = "[Party] Soothing Silence",
    level_required = 105,
    npc_start = 542,
    npc_complete = 542,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 942: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 942: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 942: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 325,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 942: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 329,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 942: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 332,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 942: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 942: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 942: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 942: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 942: Rewards given")
                self:give_rewards({
                    exp = 12417633,
                    gold = 101632,
                    items = {
                        { item_id = 30000685, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_942