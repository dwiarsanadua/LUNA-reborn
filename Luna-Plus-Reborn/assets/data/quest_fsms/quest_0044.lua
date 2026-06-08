--[[
  Quest 44: The Age Old battle
  Level Required: 60
  NPC Start: 46, NPC Complete: 46
  Rewards: EXP=437964, Gold=14970
]]

local fsm = require('fsm_engine')

local quest_44 = fsm:new({
    id = 44,
    name = "The Age Old battle",
    level_required = 60,
    npc_start = 46,
    npc_complete = 46,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 44: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 44: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 44: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 65,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 44: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 66,
                    count = 12,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 44: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 44: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 44: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 44: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 44: Rewards given")
                self:give_rewards({
                    exp = 437964,
                    gold = 14970,
                    items = {
                        { item_id = 30000008, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_44