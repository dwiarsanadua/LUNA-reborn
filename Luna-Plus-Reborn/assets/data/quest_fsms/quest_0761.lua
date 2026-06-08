--[[
  Quest 761: [Quest] Fourth Training
  Level Required: 0
  NPC Start: 402, NPC Complete: 410
  Prerequisites: [760]
  Rewards: EXP=100000, Gold=20000
]]

local fsm = require('fsm_engine')

local quest_761 = fsm:new({
    id = 761,
    name = "[Quest] Fourth Training",
    level_required = 0,
    npc_start = 402,
    npc_complete = 410,
    prerequisites = {760},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 761: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 402,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 761: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 761: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 43,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 761: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 402,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 761: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 410,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 761: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 761: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 410,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 761: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 761: Rewards given")
                self:give_rewards({
                    exp = 100000,
                    gold = 20000,
                    items = {
                        { item_id = 13000071, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_761