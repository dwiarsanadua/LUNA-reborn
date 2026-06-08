--[[
  Quest 844: It is a mystery 2
  Level Required: 85
  NPC Start: 505, NPC Complete: 505
  Prerequisites: [843]
  Rewards: EXP=2548241, Gold=57826
]]

local fsm = require('fsm_engine')

local quest_844 = fsm:new({
    id = 844,
    name = "It is a mystery 2",
    level_required = 85,
    npc_start = 505,
    npc_complete = 505,
    prerequisites = {843},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 844: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 844: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 844: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 683,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 844: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 844: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 844: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 844: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 844: Rewards given")
                self:give_rewards({
                    exp = 2548241,
                    gold = 57826,
                    items = {
                        { item_id = 30000685, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_844