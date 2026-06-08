--[[
  Quest 2006: [Daily] Cocatris
  Level Required: 52
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=104237
]]

local fsm = require('fsm_engine')

local quest_2006 = fsm:new({
    id = 2006,
    name = "[Daily] Cocatris",
    level_required = 52,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2006: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2006: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2006: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 145,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2006: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2006: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2006: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2006: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2006: Rewards given")
                self:give_rewards({
                    exp = 104237,
                    items = {
                        { item_id = 30000704, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2006