--[[
  Quest 2001: [Daily] Predator Gargoyles
  Level Required: 42
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=54351
]]

local fsm = require('fsm_engine')

local quest_2001 = fsm:new({
    id = 2001,
    name = "[Daily] Predator Gargoyles",
    level_required = 42,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2001: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2001: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2001: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 60,
                    count = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2001: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2001: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2001: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2001: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2001: Rewards given")
                self:give_rewards({
                    exp = 54351,
                    items = {
                        { item_id = 30000704, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2001