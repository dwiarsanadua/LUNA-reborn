--[[
  Quest 202: A secret medicine for undoing curse
  Level Required: 46
  NPC Start: 7, NPC Complete: 33
  Rewards: EXP=170370, Gold=9000
]]

local fsm = require('fsm_engine')

local quest_202 = fsm:new({
    id = 202,
    name = "A secret medicine for undoing curse",
    level_required = 46,
    npc_start = 7,
    npc_complete = 33,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 202: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 202: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 202: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 38,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 202: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 17,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 202: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 202: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 202: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 202: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 202: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 202: Rewards given")
                self:give_rewards({
                    exp = 170370,
                    gold = 9000,
                    items = {
                        { item_id = 139, count = 1 },
                        { item_id = 139, count = 1 },
                        { item_id = 140, count = 1 },
                        { item_id = 140, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_202