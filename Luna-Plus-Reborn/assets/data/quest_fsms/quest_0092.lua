--[[
  Quest 92: Pest Control
  Level Required: 12
  NPC Start: 54, NPC Complete: 54
  Prerequisites: [89]
  Rewards: EXP=1607, Gold=1330
]]

local fsm = require('fsm_engine')

local quest_92 = fsm:new({
    id = 92,
    name = "Pest Control",
    level_required = 12,
    npc_start = 54,
    npc_complete = 54,
    prerequisites = {89},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 92: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 54,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 92: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 92: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 8,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 92: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 29,
                    count = 8,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 92: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 54,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 92: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 92: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 54,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 92: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 92: Rewards given")
                self:give_rewards({
                    exp = 1607,
                    gold = 1330,
                    items = {
                        { item_id = 21000271, count = 3 },
                        { item_id = 21000008, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_92