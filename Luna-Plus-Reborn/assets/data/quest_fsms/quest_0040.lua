--[[
  Quest 40: Getting rid of Succubus
  Level Required: 51
  NPC Start: 39, NPC Complete: 39
  Rewards: EXP=164841, Gold=8890
]]

local fsm = require('fsm_engine')

local quest_40 = fsm:new({
    id = 40,
    name = "Getting rid of Succubus",
    level_required = 51,
    npc_start = 39,
    npc_complete = 39,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 40: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 40: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 40: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 35,
                    count = 12,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 40: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 40: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 40: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 40: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 40: Rewards given")
                self:give_rewards({
                    exp = 164841,
                    gold = 8890,
                    items = {
                        { item_id = 30000032, count = 7 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_40