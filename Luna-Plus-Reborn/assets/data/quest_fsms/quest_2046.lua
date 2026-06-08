--[[
  Quest 2046: [Daily] Supporting Nera Harbor 2
  Level Required: 96
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=3814097
]]

local fsm = require('fsm_engine')

local quest_2046 = fsm:new({
    id = 2046,
    name = "[Daily] Supporting Nera Harbor 2",
    level_required = 96,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2046: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2046: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2046: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 291,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2046: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2046: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2046: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2046: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2046: Rewards given")
                self:give_rewards({
                    exp = 3814097,
                    items = {
                        { item_id = 30000836, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2046