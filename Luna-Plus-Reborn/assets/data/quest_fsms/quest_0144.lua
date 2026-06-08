--[[
  Quest 144: [Repeatable] Bad Moon Rising
  Level Required: 23
  NPC Start: 26, NPC Complete: 26
  Prerequisites: [143]
  Rewards: EXP=10563, Gold=2500
]]

local fsm = require('fsm_engine')

local quest_144 = fsm:new({
    id = 144,
    name = "[Repeatable] Bad Moon Rising",
    level_required = 23,
    npc_start = 26,
    npc_complete = 26,
    prerequisites = {143},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 144: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 144: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 144: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 11,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 144: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 144: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 144: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 144: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 144: Rewards given")
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

return quest_144