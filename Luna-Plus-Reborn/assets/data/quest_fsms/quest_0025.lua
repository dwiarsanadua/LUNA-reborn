--[[
  Quest 25: Protect the Farm
  Level Required: 25
  NPC Start: 26, NPC Complete: 26
  Prerequisites: [22]
  Rewards: EXP=14494, Gold=2760
]]

local fsm = require('fsm_engine')

local quest_25 = fsm:new({
    id = 25,
    name = "Protect the Farm",
    level_required = 25,
    npc_start = 26,
    npc_complete = 26,
    prerequisites = {22},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 25: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 25: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 25: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 24,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 25: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 53,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 25: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 25: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 25: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 25: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 25: Rewards given")
                self:give_rewards({
                    exp = 14494,
                    gold = 2760,
                    items = {
                        { item_id = 30000825, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_25