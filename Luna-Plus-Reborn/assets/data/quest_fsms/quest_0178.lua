--[[
  Quest 178: Preparation for an Attack
  Level Required: 38
  NPC Start: 10, NPC Complete: 33
  Prerequisites: [168]
  Rewards: EXP=45930, Gold=1050
]]

local fsm = require('fsm_engine')

local quest_178 = fsm:new({
    id = 178,
    name = "Preparation for an Attack",
    level_required = 38,
    npc_start = 10,
    npc_complete = 33,
    prerequisites = {168},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 178: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 178: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 178: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 178: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 178: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 31,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 178: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 178: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 178: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 178: Rewards given")
                self:give_rewards({
                    exp = 45930,
                    gold = 1050,
                    items = {
                        { item_id = 92, count = 1 },
                        { item_id = 21000096, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_178