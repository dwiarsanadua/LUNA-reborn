--[[
  Quest 819: Sample Collection
  Level Required: 73
  NPC Start: 92, NPC Complete: 92
  Prerequisites: [818]
  Rewards: EXP=1873234, Gold=34761
]]

local fsm = require('fsm_engine')

local quest_819 = fsm:new({
    id = 819,
    name = "Sample Collection",
    level_required = 73,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {818},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 819: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 819: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 819: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 252,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 819: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 819: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 819: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 819: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 819: Rewards given")
                self:give_rewards({
                    exp = 1873234,
                    gold = 34761,
                    items = {
                        { item_id = 386, count = 1 },
                        { item_id = 386, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_819