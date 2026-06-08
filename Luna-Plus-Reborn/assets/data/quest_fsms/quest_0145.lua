--[[
  Quest 145: [Repeatable] Bad Moon Rising
  Level Required: 23
  NPC Start: 26, NPC Complete: 26
  Prerequisites: [144]
  Rewards: EXP=10563, Gold=2500
]]

local fsm = require('fsm_engine')

local quest_145 = fsm:new({
    id = 145,
    name = "[Repeatable] Bad Moon Rising",
    level_required = 23,
    npc_start = 26,
    npc_complete = 26,
    prerequisites = {144},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 145: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 145: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 145: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 11,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 145: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 145: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 145: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 145: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 145: Rewards given")
                self:give_rewards({
                    exp = 10563,
                    gold = 2500,
                    items = {
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_145