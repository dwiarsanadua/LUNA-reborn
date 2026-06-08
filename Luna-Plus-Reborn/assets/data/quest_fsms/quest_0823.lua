--[[
  Quest 823: Sample a little bit more
  Level Required: 75
  NPC Start: 92, NPC Complete: 92
  Prerequisites: [822]
  Rewards: EXP=2043968, Gold=61514
]]

local fsm = require('fsm_engine')

local quest_823 = fsm:new({
    id = 823,
    name = "Sample a little bit more",
    level_required = 75,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {822},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 823: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 823: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 823: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 251,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 823: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 823: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 823: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 823: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 823: Rewards given")
                self:give_rewards({
                    exp = 2043968,
                    gold = 61514,
                    items = {
                        { item_id = 388, count = 1 },
                        { item_id = 388, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_823