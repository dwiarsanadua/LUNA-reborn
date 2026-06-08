--[[
  Quest 36: Alker vs. Predator Gargoyles
  Level Required: 42
  NPC Start: 36, NPC Complete: 36
  Rewards: EXP=78507, Gold=4070
]]

local fsm = require('fsm_engine')

local quest_36 = fsm:new({
    id = 36,
    name = "Alker vs. Predator Gargoyles",
    level_required = 42,
    npc_start = 36,
    npc_complete = 36,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 36: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 36: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 36: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 60,
                    count = 12,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 36: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 36: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 36: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 36: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 36: Rewards given")
                self:give_rewards({
                    exp = 78507,
                    gold = 4070,
                    items = {
                        { item_id = 21000097, count = 6 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_36